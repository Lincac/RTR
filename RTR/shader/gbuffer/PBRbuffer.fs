#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gParameter;
layout (location = 4) out vec2 gVelo;

in V_out{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    
    vec4 nowPos;
    vec4 prePos;
}fs_in;

uniform sampler2D Albedo;
uniform sampler2D Normal;
uniform sampler2D Metallic;
uniform sampler2D Roughness;
uniform sampler2D Ao;

uniform float NEAR; // 投影矩阵的近平面
uniform float FAR; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}

vec3 getNormalFromMap(){
    vec3 tangentNormal = texture(Normal, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main(){
    gPosition.xyz = fs_in.FragPos;
    gPosition.a = LinearizeDepth(gl_FragCoord.z) / FAR;
    gNormal.xyz = getNormalFromMap();
    gAlbedo.rgb = texture(Albedo,fs_in.TexCoords).rgb;
    gParameter.r = texture(Metallic,fs_in.TexCoords).r;
    gParameter.g = texture(Roughness,fs_in.TexCoords).r;
    gParameter.b = texture(Ao,fs_in.TexCoords).r;

    vec2 nPos = (fs_in.nowPos.xy / fs_in.nowPos.w) * 0.5 + 0.5;
    vec2 pPos = (fs_in.prePos.xy / fs_in.prePos.w) * 0.5 + 0.5;
    gVelo.rg = nPos - pPos;
}