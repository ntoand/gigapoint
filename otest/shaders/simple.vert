#version 120

attribute vec3 VertexPosition;
attribute vec3 VertexColor;

varying vec3 vcolor;

void main()
{
	//position
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);
    //gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(VertexPosition,1.0);

    //color
    vcolor = VertexColor;
    
    //size
    vec3 posEye = vec3(gl_ModelViewMatrix * vec4(VertexPosition, 1.0));
    float dist = length(posEye);
    //gl_PointSize = pointRadius * (pointScale / dist);
    gl_PointSize = 50.0 / dist;
    //gl_PointSize = 20.0;
    
    //gl_FrontColor = gl_Color;
}
