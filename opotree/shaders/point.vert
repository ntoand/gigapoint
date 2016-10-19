#version 120

#define FIXED_POINT_SIZE

attribute vec3 VertexPosition;
attribute vec3 VertexColor;

uniform float uScreenHeight;
uniform float uSpacing;
uniform float uPointSize;
uniform float uMinPointSize;
uniform float uMaxPointSize;

//varying vec3 vViewPosition;
//varying vec3 vWorldPosition;
//varying float vLinearDepth;
varying vec3 vColor;
//varying float vRadius;

void main()
{
	vec4 worldPosition = vec4( VertexPosition, 1.0 );
	vec4 mvPosition = gl_ModelViewMatrix * vec4( VertexPosition, 1.0 );
	//vViewPosition = -mvPosition.xyz;
	//vWorldPosition = worldPosition.xyz;
    //vLinearDepth = -mvPosition.z;
    //vNormal = normalize(gl_NormalMatrix * VertexNormal);

	//position
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);

    //color
    vColor = VertexColor;
	
    //size
    float pointSize = 1.0;
    float projFactor = 2.41; //1.0 / tan(uFOV / 2.0);
    projFactor /= length(mvPosition);
    projFactor *= uScreenHeight / 2.0;
	//float r = uSpacing * 1.5;
	//vRadius = r;

	pointSize = uPointSize * projFactor;

	pointSize = max(uMinPointSize, pointSize);
	//pointSize = min(uMaxPointSize, pointSize);

	//vRadius = pointSize / projFactor;
	gl_PointSize = pointSize;
}
