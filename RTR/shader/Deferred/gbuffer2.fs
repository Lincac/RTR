#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gParameter;

in vec3 fragpos;
in vec3 normal;
in vec2 TexCoords;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2D Metallic;
uniform sampler2D Roughness;
uniform sampler2D Ao;

vec3 getNormalFromMap(){
    vec3 tangentNormal = texture(Normal, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fragpos);
    vec3 Q2  = dFdy(fragpos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main(){
    gPosition.xyz = fragpos;
    gNormal.xyz = getNormalFromMap();
    gAlbedo.rgb = texture(Albedo,TexCoords).rgb;
    gParameter.r = texture(Metallic,TexCoords).r;
    gParameter.g = texture(Roughness,TexCoords).r;
    gParameter.b = texture(Ao,TexCoords).r;
}