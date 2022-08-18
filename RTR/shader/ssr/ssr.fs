#version 450 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gParameter;

uniform sampler2D Normal;

uniform mat4 projection;
uniform mat4 view;

void main(){
    vec3 ViewPos = texture(gPosition,TexCoords).xyz;
    ViewPos = vec3(view * vec4(ViewPos,1.0));

    vec3 N = texture(gNormal,TexCoords).xyz;
    mat3 normalMatrix = transpose(inverse(mat3(view)));
    N =  normalMatrix * N;

    vec3 V = normalize(ViewPos);
    vec3 R = reflect(V,N);

    vec3 positionto = ViewPos; // 记录当前位置

    float maxDistance = 8.0; // 光线长度
    float resolution  = 0.3; // 用于计算迭代次数
    int   steps       = 5;  // 二分搜索步长
    float thickness   = 0.2;  // 误差范围

    vec2 texSize = textureSize(gPosition,0).xy; // 该纹理为屏幕大小的纹理
    vec4 uv = vec4(0.0);

    float roughness = texture(gParameter,TexCoords).g; // 粗糙的物体不反射
    if(roughness > 0.8){
        FragColor = uv;
        uv.ba = vec2(0.0);
        return;
    }

    if(dot(-V,N) > 0.72){
        FragColor = uv;
        uv.ba = vec2(0.0);
        return;
    }

    vec3 startView = ViewPos; // 观测空间中
    vec3 endView = ViewPos + R * maxDistance;

    vec4 startFrag = projection * vec4(startView,1.0); 
    startFrag.xyz = startFrag.xyz / startFrag.w; // NDC坐标 (-1,1)
    startFrag.xy = startFrag.xy * 0.5 + 0.5;   // uv坐标 (0,1)
    startFrag.xy *= texSize; // 屏幕上的像素坐标

    vec4 endFrag = projection * vec4(endView,1.0); 
    endFrag.xyz = endFrag.xyz / endFrag.w;
    endFrag.xyz = endFrag.xyz * 0.5 + 0.5;
    endFrag.xy *= texSize;

    vec2 frag = startFrag.xy; // 在屏幕上像素坐标下的当前位置
    uv.xy = frag / texSize;  // 对应的纹理坐标

    float deltaX = endFrag.x - startFrag.x; // 屏幕上像素差
    float deltaY = endFrag.y - startFrag.y;
    float axis = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0; // 判定使用较长的轴
    float delta = mix(abs(deltaY),abs(deltaX),axis) * clamp(resolution,0.0,1.0); // 迭代次数
    vec2 increment = vec2(deltaX,deltaY) / max(delta,0.001); // 计算增量    

    float search0 = 0.0;
    float search1 = 0.0; // 当前位置所代表的线的百分比

    int hit0 = 0;  // 记录第一次击中点（粗略搜索）
    int hit1 = 0;  // 记录第二次击中点（精细搜索）

    float viewDistance = startView.z; // 观测空间下当前深度
    float depth = thickness; // 深度差

    for(int i = 0 ; i < int(delta) ; ++i){
        frag += increment;
        uv.xy = frag / texSize;
        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;

        positionto = texture(gPosition,uv.xy).xyz; // (0.1,100) 观测空间下的点
        positionto = vec3(view * vec4(positionto,1.0));

        search1 = mix((frag.y - startFrag.y) / deltaY,(frag.x - startFrag.x) / deltaX,axis);

        viewDistance = (startView.z * endView.z) / mix(endView.z,startView.z,search1); // 在开始和结束片段中插值深度（透视插值）
        depth = viewDistance - positionto.z; // 计算当前观察点和采样点的深度差
        depth *= -1.0;  //  视图空间中的z轴在负半轴，所有的深度都是负数

        if(depth > 0.0 && depth < thickness){ // 深度差在一定范围内，说明相交
            hit0 = 1;
            break; // 停止搜索
        }else{
            search0 = search1; // 继续搜索
        }
    }

    search1 = search0 + ((search1 - search0) / 2.0); // 取中间值
    steps *= hit0;// 如果击中了就继续搜索

    for(int i = 0; i < steps ;++i){  // 二分搜索
        frag = mix(startFrag.xy,endFrag.xy,search1);
        uv.xy = frag / texSize;
        positionto = texture(gPosition,uv.xy).xyz; 
        positionto = vec3(view * vec4(positionto,1.0));

        viewDistance = (startView.z * endView.z) / mix(endView.z,startView.z,search1); 
        depth = viewDistance - positionto.z; 
        depth *= -1.0;

        if(depth > 0.0 && depth < thickness){  // 左边寻找
            hit1 = 1; 
            search1 = search0 + ((search1 - search0) / 2.0);
        }else{                               // 右边寻找
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2.0);
            search0 = temp;
        }
    }

                    //w分量为0代表不在纹理中   可见度受视线和反射的夹角影响     因为找不到确切的相交点，只能依据距离占比设置可见度，越远越看不到         同时受影响的还有反射的距离，越远越看不到
    float visibility = hit1 * (1.0 - max(dot(-V,R),0.0)) * (1.0 - clamp(depth / thickness,0,1))                           * (1.0 - clamp(length(positionto.xyz - ViewPos) / maxDistance,0,1))
    * (uv.x < 0.0 || uv.x > 1.0 ? 0 : 1) * (uv.y < 0.0 || uv.y > 1.0 ? 0 : 1) * (1.0 - roughness);

    // float visibility = hit1 * (1.0 - roughness);

    uv.ba = vec2(visibility);

    FragColor = uv;
}