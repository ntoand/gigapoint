#version 120

attribute vec3 VertexPosition;

//uniform mat4 ModelViewMatrix;
//uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition,1.0);
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);
    //gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(VertexPosition,1.0);

    gl_PointSize = 2.0;
}
