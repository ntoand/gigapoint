#version 120

//#define SQUARE_POINT_SHAPE
#define CIRCLE_POINT_SHAPE

varying vec3 vViewPosition;
varying vec3 vWorldPosition;
varying float vLinearDepth;
varying vec3 vColor;
varying float vRadius;

void main() {
	
#if defined SQUARE_POINT_SHAPE
	gl_FragColor = vec4(vColor, 1.0);
	return;
#endif

	vec2 cxy = gl_PointCoord * 2.0 - vec2(1.0);    
   	float r = dot(cxy, cxy);
   	if (r > 1.0) 
   		discard;



    gl_FragColor = vec4(vColor, 1.0);  	
}
