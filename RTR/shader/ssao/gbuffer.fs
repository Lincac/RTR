#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 fragpos;
in vec3 normal;
in vec2 TexCoords;

uniform float roughness;

void main(){
    gPosition.xyz = fragpos;
    gNormal = normalize(normal);
    gAlbedo.xyz = vec3(0.94);
    gAlbedo.w = roughness;
}