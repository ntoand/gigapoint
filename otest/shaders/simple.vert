#version 120

attribute vec3 VertexPosition;
attribute vec3 VertexColor;

varying vec3 vcolor;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);
    //gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(VertexPosition,1.0);

    vcolor = VertexColor;
}
