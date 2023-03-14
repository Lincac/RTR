#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdr;
uniform sampler2D ssaoblur;
uniform sampler2D ssr;
uniform float exposure;

void main(){
    vec2 uv = texture(ssr,TexCoords).xy;
    float visibility = texture(ssr,TexCoords).b;   
    visibility = clamp(visibility,0,1); 
    vec3 ssrcolor = vec3(0.0);
    vec2 texelSize =  1.0 / vec2(textureSize(ssr,0));
    for(int i = -2;i<=2;i++){
        for(int j = -2;j<=2;j++){
            vec2 offset = vec2(float(i),float(j)) * texelSize;
            ssrcolor += mix(vec3(0.0),texture(hdr,uv).rgb,visibility);
        }
    }
    ssrcolor /= (5.0 * 5.0);
    vec3 hdrcolor = texture(hdr,TexCoords).rgb;
    float ao = texture(ssaoblur,TexCoords).r;
    hdrcolor += ssrcolor;
    hdrcolor *= ao;

    vec3 mapped = vec3(1.0) - exp(-hdrcolor * exposure);   // hdr to ldr
    // vec3 mapped = hdrcolor / (hdrcolor + vec3(1.0));

    const float gamma = 2.2;
    mapped = pow(mapped,vec3(1.0 / gamma));

    FragColor = vec4(mapped,1.0); 
    // FragColor = texture(hdr,uv) * visibility;
}