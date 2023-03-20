#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D currentcolor;
uniform sampler2D historycolor;
uniform sampler2D gVelo;
uniform sampler2D gPosition;

vec3 RGB2YCoCgR(vec3 rgbColor)
{
    vec3 YCoCgRColor;

    YCoCgRColor.y = rgbColor.r - rgbColor.b;
    float temp = rgbColor.b + YCoCgRColor.y / 2;
    YCoCgRColor.z = rgbColor.g - temp;
    YCoCgRColor.x = temp + YCoCgRColor.z / 2;

    return YCoCgRColor;
}

vec3 YCoCgR2RGB(vec3 YCoCgRColor)
{
    vec3 rgbColor;

    float temp = YCoCgRColor.x - YCoCgRColor.z / 2;
    rgbColor.g = YCoCgRColor.z + temp;
    rgbColor.b = temp - YCoCgRColor.y / 2;
    rgbColor.r = rgbColor.b + YCoCgRColor.y;

    return rgbColor;
}

float Luminance(vec3 color)
{
    return 0.25 * color.r + 0.5 * color.g + 0.25 * color.b;
}

vec3 ToneMap(vec3 color)
{
    return color / (1 + Luminance(color));
}

vec3 UnToneMap(vec3 color)
{
    return color / (1 - Luminance(color));
}

vec2 getClosestOffset()
{
    vec2 deltaRes = 1.0 / textureSize(gPosition,0);
    float closestDepth = 1.0f;
    vec2 closestUV = TexCoords;

    for(int i=-1;i<=1;++i)
    {
        for(int j=-1;j<=1;++j)
        {
            vec2 newUV = TexCoords + deltaRes * vec2(i, j);

            float depth = texture2D(gPosition, newUV).w;

            if(depth < closestDepth)  // 因为velo也有锯齿所以要找没有锯齿的部分，而没有锯齿的部分一般都在深度近的地方？（锯齿都在边缘）
            {
                closestDepth = depth;
                closestUV = newUV;
            }
        }
    }

    return closestUV;
}

void main(){
    vec3 cur = texture(currentcolor,TexCoords).rgb;

    vec2 hisuv = TexCoords - texture(gVelo,getClosestOffset()).rg;
    vec3 his = texture(historycolor,hisuv).rgb;

    cur = RGB2YCoCgR(ToneMap(cur));
    his = RGB2YCoCgR(ToneMap(his));

    vec3 aabbMin = vec3(0.0);
    vec3 aabbMax = vec3(0.0);
    vec3 m1 = vec3(0.0);
    vec3 m2 = vec3(0.0);

    vec2 texSize = 1.0 / textureSize(currentcolor,0).xy;

    for(int i = -1 ; i <= 1 ;i++){
        for(int j = -1 ; j <= 1 ;j++){
            vec3 color = texture(currentcolor,TexCoords + vec2(i,j) * texSize).rgb;
            color = RGB2YCoCgR(ToneMap(color));
            m1 += color;
            m2 += color * color;
        }
    }
    
    const int N = 9;
    const float VarianceClipGamma = 1.0f;
    vec3 mu = m1 / N;
    vec3 sigma = sqrt(abs(m2 / N - mu * mu));
    aabbMin = mu - VarianceClipGamma * sigma;
    aabbMax = mu + VarianceClipGamma * sigma;

    // 颜色截断就是把上一帧的颜色，限制在当前颜色的范围内（在当前像素附近范围内找最大最小值，相当于AABB）
    vec3 p_clip = 0.5 * (aabbMax + aabbMin);
    vec3 e_clip = 0.5 * (aabbMax - aabbMin);

    vec3 v_clip = his - p_clip;
    vec3 v_unit = v_clip.xyz / e_clip;
    vec3 a_unit = abs(v_unit);
    float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

    if (ma_unit > 1.0)
        his = p_clip + v_clip / ma_unit;

    cur = UnToneMap(YCoCgR2RGB(cur));
    his = UnToneMap(YCoCgR2RGB(his));

    FragColor = vec4(mix(cur,his,0.95),1.0);
}