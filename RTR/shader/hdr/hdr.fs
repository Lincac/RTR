#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D HDR;
uniform sampler2D SSAO;
uniform sampler2D SSR;

uniform float exposure;
uniform bool OpenSSAO;
uniform bool OpenSSR;

void main(){
    vec3 hdrcolor = texture(HDR,TexCoords).rgb;

    if(OpenSSR){
        vec3 ssrcolor = texture(SSR,TexCoords).rgb;
        hdrcolor += ssrcolor;
        hdrcolor += ssrcolor;
    }

    if(OpenSSAO){
        float ao = texture(SSAO,TexCoords).r;
        hdrcolor *= ao;
    }

    vec3 mapped = vec3(1.0) - exp(-hdrcolor * exposure);   // hdr to ldr
    // vec3 mapped = hdrcolor / (hdrcolor + vec3(1.0));

    const float gamma = 2.2;
    mapped = pow(mapped,vec3(1.0 / gamma));

    FragColor = vec4(mapped,1.0); 
    // FragColor = texture(hdr,uv) * visibility;
}