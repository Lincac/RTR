#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D Tex;
uniform sampler2D Tex2;
// uniform sampler2DArray Tex;

float near = 0.1; 
float far  = 500.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}


void main(){
    float t = texture(Tex,TexCoords,4).r;
    FragColor = texture(Tex,TexCoords) + texture(Tex2,TexCoords);
}