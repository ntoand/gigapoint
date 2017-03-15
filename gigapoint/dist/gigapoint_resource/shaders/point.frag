varying vec3 vColor;
#if defined FILTER_EDL
varying float vDepth;
#endif

void main() {

  	gl_FragColor = vec4(vColor,1.0);
#if defined FILTER_EDL
  	gl_FragColor.a = vDepth;
#endif

#if defined CIRCLE_POINT_SHAPE
    vec3 N;
    N.xy = gl_PointCoord* 2.0 - vec2(1.0);    
    float mag = dot(N.xy, N.xy);
    if (mag > 1.0) {
        discard;
    }
#endif

}
