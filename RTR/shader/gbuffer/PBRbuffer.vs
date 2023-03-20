#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out V_out{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec4 nowPos;
    vec4 prePos;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec2 Halton_2_3[8] =
{
    vec2(0.0f, -1.0f / 3.0f),
    vec2(-1.0f / 2.0f, 1.0f / 3.0f),
    vec2(1.0f / 2.0f, -7.0f / 9.0f),
    vec2(-3.0f / 4.0f, -1.0f / 9.0f),
    vec2(1.0f / 4.0f, 5.0f / 9.0f),
    vec2(-1.0f / 4.0f, -5.0f / 9.0f),
    vec2(3.0f / 4.0f, 1.0f / 9.0f),
    vec2(-7.0f / 8.0f, 7.0f / 9.0f)
};

uniform float scr_width;
uniform float scr_height;
uniform int offsetindex;

uniform mat4 preProjection;
uniform mat4 preView;
uniform mat4 preModel;

void main(){
    vs_out.nowPos = projection * view * model * vec4(aPos,1.0);
    vs_out.prePos = preProjection * preView * preModel * vec4(aPos,1.0);

    vs_out.FragPos = vec3(model * vec4(aPos,1.0));
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalMatrix * aNormal;
    vs_out.TexCoords = aTexCoords;

    vec2 texSize = vec2(1.0 / scr_width,1.0 / scr_height);
    vec2 offset = Halton_2_3[offsetindex] * texSize;
    mat4 temp = projection;
    temp[2][0] += offset.x;
    temp[2][1] += offset.y;

    gl_Position = temp * view * model * vec4(aPos,1.0); 
}