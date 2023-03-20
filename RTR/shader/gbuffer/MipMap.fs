#version 450 core
 
uniform sampler2D u_depthBuffer;
uniform int u_previousLevel;
uniform ivec2 u_previousLevelDimensions;
 
in vec2 TexCoord;

void main( ) {
    ivec2 thisLevelTexelCoord = ivec2(gl_FragCoord);
	ivec2 previousLevelBaseTexelCoord = 2 * thisLevelTexelCoord;
	vec4 depthTexelValues;
	depthTexelValues.x = texelFetch(u_depthBuffer,
                                    previousLevelBaseTexelCoord,
                                    u_previousLevel).r;
	depthTexelValues.y = texelFetch(u_depthBuffer,
                                    previousLevelBaseTexelCoord + ivec2(1, 0),
                                    u_previousLevel).r;
	depthTexelValues.z = texelFetch(u_depthBuffer,
                                    previousLevelBaseTexelCoord + ivec2(1, 1),
                                    u_previousLevel).r;
	depthTexelValues.w = texelFetch(u_depthBuffer,
                                    previousLevelBaseTexelCoord + ivec2(0, 1),
                                    u_previousLevel).r;

	float minDepth = min(min(depthTexelValues.x, depthTexelValues.y),
                         min(depthTexelValues.z, depthTexelValues.w));
	bool shouldIncludeExtraColumnFromPreviousLevel = ((u_previousLevelDimensions.x & 1) != 0);
	bool shouldIncludeExtraRowFromPreviousLevel = ((u_previousLevelDimensions.y & 1) != 0);
	if (shouldIncludeExtraColumnFromPreviousLevel) {
		vec2 extraColumnTexelValues;
		extraColumnTexelValues.x = texelFetch(u_depthBuffer,
                                              previousLevelBaseTexelCoord + ivec2(2, 0),
                                              u_previousLevel).r;
		extraColumnTexelValues.y = texelFetch(u_depthBuffer,
                                              previousLevelBaseTexelCoord + ivec2(2, 1),
                                              u_previousLevel).r;

		// In the case where the width and height are both odd, need to include the
        // 'corner' value as well.
		if (shouldIncludeExtraRowFromPreviousLevel) {
			float cornerTexelValue = texelFetch(u_depthBuffer,
                                                previousLevelBaseTexelCoord + ivec2(2, 2),
                                                u_previousLevel).r;
			minDepth = min(minDepth, cornerTexelValue);
		}
		minDepth = min(minDepth, min(extraColumnTexelValues.x, extraColumnTexelValues.y));
	}
	if (shouldIncludeExtraRowFromPreviousLevel) {
		vec2 extraRowTexelValues;
		extraRowTexelValues.x = texelFetch(u_depthBuffer,
                                           previousLevelBaseTexelCoord + ivec2(0, 2),
                                           u_previousLevel).r;
		extraRowTexelValues.y = texelFetch(u_depthBuffer,
                                           previousLevelBaseTexelCoord + ivec2(1, 2),
                                           u_previousLevel).r;
		minDepth = min(minDepth, min(extraRowTexelValues.x, extraRowTexelValues.y));
	}
    gl_FragDepth = minDepth;
}