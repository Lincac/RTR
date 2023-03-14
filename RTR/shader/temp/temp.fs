#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Tex;
// uniform sampler2DArray Tex;

void main(){
    //float r = texture(Tex,vec3(TexCoords,0)).r;
    FragColor = vec4(vec3(r),1.0);
}