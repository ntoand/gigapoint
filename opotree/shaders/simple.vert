#version 120

attribute vec3 VertexPosition;
attribute vec3 VertexColor;

uniform mat4 MVP;

varying vec3 vcolor;

void main()
{
    gl_Position = MVP * vec4(VertexPosition,1.0);
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);
    //gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(VertexPosition,1.0);

    gl_PointSize = 2.0;

    vcolor = VertexColor;
}
