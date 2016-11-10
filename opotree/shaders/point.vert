#version 120

attribute vec3 VertexPosition;
attribute vec3 VertexColor;

uniform float uScreenHeight;
uniform float uSpacing;
uniform float uPointSize;
uniform float uMinPointSize;
uniform float uMaxPointSize;

varying vec3 vColor;

void main()
{
	vec4 worldPosition = vec4( VertexPosition, 1.0 );
	vec4 mvPosition = gl_ModelViewMatrix * vec4( VertexPosition, 1.0 );

	//position
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);

    //color
    vColor = VertexColor;

    //size
    float pointSize = 1.0;

#if defined FIXED_POINT_SIZE
	pointSize = uPointSize*10;

#else
    float projFactor = 2.41; //1.0 / tan(uFOV / 2.0);
    projFactor /= length(mvPosition);
    projFactor *= uScreenHeight / 2.0;
	pointSize = uPointSize * projFactor;
	
#endif 
	pointSize = max(uMinPointSize, pointSize);
	pointSize = min(uMaxPointSize, pointSize);
	gl_PointSize = pointSize;
}
