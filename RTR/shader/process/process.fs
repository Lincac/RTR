#version 450 core

out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D HDR;
uniform sampler2D SSAO;
uniform sampler2D SSR;

uniform float exposure;

uniform bool OpenSSAO;
uniform bool OpenSSR;

void main(){
    vec3 hdrcolor = texture(HDR,texcoord).rgb;
    vec3 mapped = vec3(1.0) - exp(-hdrcolor * exposure);

    const float gamma = 2.2;
    mapped = pow(mapped,vec3(1.0 / gamma));

    FragColor = vec4(mapped,1.0); 
}