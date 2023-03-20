#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2DArray shadowMap;

uniform vec3 lightCol;
uniform vec3 lightDir;

uniform vec3 Ambient;
uniform float Gloss;

uniform vec3 viewPos;
uniform float farPlane;
uniform mat4 view;

uniform int cascadeCount;
uniform float cascadePlaneDistances[16];

layout (std140,binding = 0 ) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

float RadicalInverse_VdC(uint bits) // (0,1) 低差异序列
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float get_average_field(vec3 currentPos,vec2 texelSize,int layer){
    int step = 3;
    float over_depth = 0.0;
    float sum_depth = 0.0;
    for(int x = -step ; x < step ;++x){
        for(int y = -step ; y < step ;++y){
            float sampleDepth = texture(shadowMap, vec3(currentPos.xy + vec2(x,y) * texelSize,layer)).r;
            if(sampleDepth < currentPos.z){
                over_depth += sampleDepth;
                sum_depth += 1;
            }
        }
    }
    if(over_depth > 0.0) return over_depth / sum_depth;
    return 0.0;
}

float ShadowCalculation(vec3 fragPosWorldSpace)
{
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace,1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for(int i = 0; i < cascadeCount ; i++){
        if(depthValue < cascadePlaneDistances[i]){
            layer = i;
            break;
        }
    }
    if(layer == -1) layer = cascadeCount;

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace,1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float current = projCoords.z;
    if(current > 1.0) return 0.0;

    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal,normalize(lightDir))),0.06);
    if(layer == cascadeCount) bias *= 1.0 / (farPlane * 5); 
    else bias *= 1.0 / (cascadePlaneDistances[layer] * 5); // 缩小偏移量

    // pcss
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap,0));

    float avg_depth = get_average_field(projCoords,texelSize,layer);  // 滤核小于目前点的平均深度
    if(avg_depth == 0.0) return 0.0;
    float d_blocker = avg_depth;
    float d_receiver = current;
    float filt = (d_receiver - d_blocker) * 10 * 2 / d_blocker;
    if(filt == 0) return 0.0;

    // pcf
    filt = filt < 1.0 ? 2.0 : filt;
    float shadow = 0.0;
    int cout = 0;
    for(uint x = 0; x <= 4; x++)
    {
        cout++;
        for(uint y = 0; y <= 4; y++)
        {
            float Hx = RadicalInverse_VdC(x);
            float Hy = RadicalInverse_VdC(y);
            if(x / 2 == 0) Hx *= -1;
            if(y / 2 == 0) Hy *= -1;
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(Hx * filt,Hy * filt) * texelSize, layer)).r;
            shadow += (current - bias) > pcfDepth ? 1.0 : 0.0;      
        }    
    }

    shadow /= cout * cout;
    if(projCoords.z>1.0) shadow = 0.0;
    
    return shadow; 
}

void main()
{           
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  

    vec3 albedo = pow(texture(Albedo,fs_in.TexCoords).rgb,vec3(2.2));
    // ambient
    vec3 ambient = Ambient * albedo;
    // diffuse
    vec3 diffuse = lightCol * albedo * max(dot(lightDir, normal), 0.0);;
    // specular
    vec3 specular = lightCol * albedo * pow(max(dot(normal, halfwayDir), 0.0), Gloss);    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPos);                      

    FragColor = vec4((ambient + (1.0 - shadow) * (diffuse + specular)), 1.0);
    // FragColor = vec4(fs_in.FragPos,1.0);
}