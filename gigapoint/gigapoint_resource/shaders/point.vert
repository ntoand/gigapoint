attribute vec3 VertexPosition;
attribute vec3 VertexColor;

uniform sampler2D uColorTexture;
uniform vec2 uHeightMinMax;
uniform float uScreenHeight;
uniform float uPointScale;
uniform vec2 uPointSizeRange;

varying vec3 vColor;

void main()
{
	//position
    gl_Position = gl_ModelViewProjectionMatrix * vec4(VertexPosition,1.0);
    vec3 mvPosition = (gl_ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;

    //color
    vColor = vec3(VertexColor.x / 255.0, VertexColor.y/255.0, VertexColor.z/255.0) ;
#if defined MATERIAL_ELEVATION
    float w = (VertexPosition.z - uHeightMinMax[0]) / (uHeightMinMax[1]-uHeightMinMax[0]);
    vColor = texture2D(uColorTexture, vec2(w,0.5));
#endif
        
    //size
    float pointSize = 1.0;

#if defined FIXED_POINT_SIZE
    pointSize = uPointScale*10;

#else
    float projFactor = 2.41; //1.0 / tan(uFOV / 2.0);
    projFactor /= length(mvPosition);
    projFactor *= uScreenHeight / 2.0;
    pointSize = uPointScale * projFactor;

#endif
    pointSize = max(uPointSizeRange[0], pointSize);
    pointSize = min(uPointSizeRange[1], pointSize);
    gl_PointSize = pointSize;    
}
