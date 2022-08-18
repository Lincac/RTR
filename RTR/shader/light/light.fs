#version 450 core

out vec4 FragColor;

uniform vec3 lightcolor;

void main(){
    FragColor = vec4(lightcolor,1.0);
}