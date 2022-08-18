#version 460 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D ssao;

void main(){
    vec2 texelSize =  1.0 / vec2(textureSize(ssao,0));
    float result = 0.0;
    for(int i = -2;i<=2;i++){
        for(int j = -2;j<=2;j++){
            vec2 offset = vec2(float(i),float(j)) * texelSize;
            result += texture(ssao,TexCoords + offset).r;
        }
    }
    FragColor = result / (5.0 * 5.0);
}