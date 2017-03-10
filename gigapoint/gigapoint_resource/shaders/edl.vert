attribute vec3 VertexPosition;
attribute vec3 VertexTexCoord;

varying vec3 vPos;
varying vec3 vUV;

void main()
{
    gl_Position = vec4(VertexPosition, 1.0);
    vPos = VertexPosition;
    vUV = VertexTexCoord;
}
