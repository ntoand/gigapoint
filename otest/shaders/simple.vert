#version 120

void main()
{
	//position
    gl_Position = ftransform();

    //color
    gl_FrontColor = gl_Color;
    
    //size
    vec3 posEye = (gl_ModelViewMatrix * gl_Vertex).xyz;
    float dist = length(posEye);
    gl_PointSize = 50.0 / dist;
}
