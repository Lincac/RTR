#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT{
    vec3 fragpos;
    vec2 TexCoords;
    vec3 normal;
}vs_out;

void main(){
    gl_Position = projection * view * model * vec4(aPos,1.0);

    vs_out.fragpos = vec3(model * vec4(aPos,1.0));
    vs_out.TexCoords = aTexCoords;
    //vs_out.normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.normal = mat3(model) * aNormal;
}
