uniform sampler2D uColorTexture;

void main()
{
	//position
    gl_Position = ftransform();

    //color
    gl_FrontColor = gl_Color;
    gl_FrontColor = texture2D(uColorTexture, vec2(0.5,0.5));
    
    //size
    vec3 posEye = (gl_ModelViewMatrix * gl_Vertex).xyz;
    float dist = length(posEye);
    float pointsize = 100.0 / dist;
    pointsize = max(pointsize, 2);
    pointsize = min(pointsize, 50);
    gl_PointSize = pointsize;
}
