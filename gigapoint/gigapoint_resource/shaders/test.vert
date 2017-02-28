uniform sampler2D uColorTexture;
uniform vec2 uHeightMinMax;
uniform float uScreenHeight;
uniform float uPointSize;
uniform float uMinPointSize;
uniform float uMaxPointSize;

void main()
{
	//position
    gl_Position = ftransform();
    vec3 mvPosition = (gl_ModelViewMatrix * gl_Vertex).xyz;

    //color
    gl_FrontColor = gl_Color;
#if defined MATERIAL_ELEVATION
    float w = (gl_Vertex.z - uHeightMinMax[0]) / (uHeightMinMax[1]-uHeightMinMax[0]);
    gl_FrontColor = texture2D(uColorTexture, vec2(w,0.5));
#endif

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
