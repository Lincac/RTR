#version 460 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 sampler[64];
uniform mat4 projection;
uniform mat4 view;
uniform float SCR_WIDTH;
uniform float SCR_HEIGHT;

const vec2 noiseScale = vec2(SCR_WIDTH / 4.0,SCR_HEIGHT / 4.0);
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

void main(){
    vec3 fragpos = texture(gPosition,TexCoords).xyz;
    fragpos = vec3(view * vec4(fragpos,1.0));

    vec3 normal = normalize(texture(gNormal,TexCoords). xyz);
    mat3 normalMatrix = transpose(inverse(mat3(view)));
    normal =  normalMatrix * normal;
    
    vec3 randomVEC = normalize(texture(noiseTexture,TexCoords*noiseScale).xyz);

    vec3 T = normalize(randomVEC - normal*dot(randomVEC,normal));  // normal*dot(randomVEC,normal) normal上的投影长度
    vec3 B = cross(normal,T);
    mat3 TBN = mat3(T,B,normal);

    float occlusion = 0;
    for(int i = 0; i<kernelSize;i++){
        vec3 samplepos = TBN * sampler[i]; 
        samplepos = fragpos + radius * samplepos;//观测空间中的采样点

        vec4 offset = vec4(samplepos,1.0);
        offset = projection * offset;  // 裁剪坐标中的采样点
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth =  vec3(view * vec4(texture(gPosition,offset.xy).xyz,1.0)).z;// 采样片段的深度
        float rangeCheck = smoothstep(0.0,1.0,radius / abs(fragpos.z - sampleDepth));
        occlusion += (sampleDepth >= samplepos.z + bias? 1.0 : 0.0) * rangeCheck;
        ///                            当前片段深度 
    }

    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}