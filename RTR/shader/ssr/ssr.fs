#version 450 core

out vec4 FragColor;

#define INV_PI 0.31830988618
#define MAX_THICKNESS 0.0017

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D depthMap;
uniform sampler2D gParameter;

uniform int maxLevel;
uniform mat4 projection;
uniform mat4 view;
// 纹理空间里进行ray trace 

bool crossedCellBoundary(vec2 oldCellIdx,vec2 newCellIdx){  // 判断是不是在同一个单元格 返回是则在不同单元格
    return (oldCellIdx.x!=newCellIdx.x)||(oldCellIdx.y!=newCellIdx.y);
}

vec2 getCellCount(int level){  // 获取当前层的分辨率
    return textureSize(depthMap,level);
}

vec2 getCell(vec2 pos,vec2 startCellCount){ // 获取当前位置窗口坐标
    return vec2(floor(pos * startCellCount));
}

vec3 intersectDepthPlane(vec3 o,vec3 d,float t){ // 获取当前光线位置（纹理空间中）
    return o + t * d;
}
float getMinimumDepthPlane(vec2 pos , int level){ // 获取当前位置深度
    return textureLod(depthMap,pos,level).r ;
}
vec3 GetScreenCoord(vec3 p){
	vec4 positionInScreen = projection*vec4(p,1.0);
	positionInScreen.xyzw /= positionInScreen.w;
	positionInScreen.xyz = positionInScreen.xyz*0.5+0.5;
    return positionInScreen.xyz;
}
vec3 intersectCellBoundary(vec3 o,vec3  d, vec2 rayCell,vec2 cell_count, vec2 crossStep, vec2 crossOffset){
    //  当前位置的深度值
    vec2 nextPos = rayCell + crossStep; // 下一个窗口坐标位置
    nextPos = nextPos/cell_count; // 纹理坐标
    nextPos = nextPos+crossOffset; // 偏移
    // 步进单元格

    vec2 dis  = nextPos - o.xy;

    vec2 delta = dis/d.xy;

    float t = min(delta.x,delta.y);

    return intersectDepthPlane(o,d,t);

}

vec3 FindIntersection(vec3 start,vec3 rayDir,float maxTraceDistance,vec3 hitPos){
    vec2 crossStep = vec2(rayDir.x>=0?1:-1,rayDir.y>=0?1:-1); // 判断步进的方向
    vec2 crossOffset = crossStep / vec2(1024,1024) / 128; // 防止光线擦边边界，添加一个偏移量
    crossStep = clamp(crossStep,0.0,1.0);

    vec3 ray = start; // 起点 （纹理坐标） 用来光线步进的
    float minZ = ray.z;
    float maxZ = ray.z+rayDir.z*maxTraceDistance;
    float deltaZ = (maxZ-minZ);  // 光线所能达到的最大深度差

    vec3 o = ray;// 起点  用来判断当前进行到哪的（纹理空间）
    vec3 d = rayDir*maxTraceDistance; // 偏移距离

    int startLevel = 2;
    int stopLevel = 0;
    vec2 startCellCount = getCellCount(startLevel); // 起始单元分辨率


    vec2 rayCell = getCell(ray.xy,startCellCount); // 当前单元的位置
    ray = intersectCellBoundary(o, d, rayCell, startCellCount, crossStep, crossOffset * 64);// 步进光线,获得当前光线在纹理空间中的位置
    // 避免自相交

    int level = startLevel;
    int iter = 0;
    bool isBackwardRay = rayDir.z<0;
    // 表示光线前向传播还是往摄像机方向（后向）传播
    
    float Dir = isBackwardRay ? -1 : 1;

    while( level>=stopLevel && ray.z*Dir <= maxZ*Dir && iter<100){ //  迭代超过100次退出，小于0层退出，当前光线大于最大深度退出
        vec2 cellCount = getCellCount(level);
        vec2 oldCellIdx = getCell(ray.xy,cellCount); // 当前窗口坐标

        float cell_minZ = getMinimumDepthPlane((oldCellIdx + vec2(0.5)) / cellCount, level);// 获得当前位置当前层级上的深度

        vec3 tmpRay = ((cell_minZ>ray.z) && !isBackwardRay) ? intersectDepthPlane(o,d,(cell_minZ-minZ)/deltaZ) : ray;
        //  这里判断如果深度小于附近的最小深度说明没有相交，则我们可以移动附近最小深度的位置去（可能还在同一单元格里面）

        vec2 newCellIdx = getCell(tmpRay.xy,cellCount);

        float thickness = level == 0 ? (ray.z-cell_minZ) : 0;
        bool crossed  = (isBackwardRay&&(cell_minZ>ray.z))||(thickness > MAX_THICKNESS)|| crossedCellBoundary(oldCellIdx, newCellIdx);// 判断是否继续步进光线
        // 如果层级大于0并且深度最小值就是当前的光线所在的单元格子并且是光线为正方向则当前格子有可能已经相交
        
        ray = crossed ? intersectCellBoundary(o, d, oldCellIdx, cellCount, crossStep, crossOffset):tmpRay; // 
        level = crossed ? min(maxLevel,level+1):level-1;//层级加，或者减
        ++iter;
    }
    bool intersected = (level < stopLevel);
    hitPos = ray;

    vec3 color = texture(gAlbedo,hitPos.xy).rgb;
    if(color.x < 0.2 && color.y < 0.2 && color.z < 0.2){
        intersected = false;
    }
	
    return intersected ? texture(gAlbedo,hitPos.xy).rgb:vec3(0.0);
}

// 纹理空间中的z = 裁剪空间中的z
void main(){
    float mask = texture(gParameter,TexCoords).r;
    if(mask == 0){
        FragColor = vec4(0.0);
        return;
    }

    vec3 WorldPos = texture(gPosition,TexCoords).xyz;
    vec3 WorldNormal = normalize(texture(gNormal,TexCoords). xyz);
    mat3 normalMatrix = transpose(inverse(mat3(view)));
    
    vec3 positionInView = vec3(view * vec4(WorldPos,1.0));
    vec3 normalInView = normalMatrix * WorldNormal;
    vec3 V = normalize(positionInView);
    vec3 relfectDir = normalize(reflect(V,normalInView));
    
    positionInView += normalInView * 0.001;

    float wi = dot(relfectDir,normalInView);
    if(wi > 0.99){
        FragColor = vec4(0.0);
        return;
    }

    vec3 endPosInView = positionInView + relfectDir * 1000;

    // 纹理空间中起点和终点  (都在0层上)
    vec3 start = GetScreenCoord(positionInView);
    vec3 endp = GetScreenCoord(endPosInView);

    vec3 rayDir = normalize(endp - start);

    float maxTraceX = rayDir.x>=0 ? (1-start.x)/rayDir.x:-start.x/rayDir.x; // 光线是正向还是反向问题
    float maxTraceY = rayDir.y>=0 ? (1-start.y)/rayDir.y:-start.y/rayDir.y;
    float maxTraceZ = rayDir.z>=0 ? (1-start.z)/rayDir.z:-start.z/rayDir.z;
    // 利用aabb包围盒思想，只要某一个最近维度到达之后，光线就要停止
    float maxTraceDistance = min(maxTraceX,min(maxTraceY,maxTraceZ));// 最大运动时间

    vec3 hitPos = vec3(0.0);

    vec3 L_indir = FindIntersection(start,rayDir,maxTraceDistance,hitPos) * INV_PI;

    FragColor = vec4(L_indir,1.0);
}