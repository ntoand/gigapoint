void main() {
  
#if defined SQUARE_POINT_SHAPE
    gl_FragColor = gl_Color;
    return;
#endif

    gl_FragColor = gl_Color;
    vec3 N;
    N.xy = gl_PointCoord* 2.0 - vec2(1.0);    
    float mag = dot(N.xy, N.xy);
    if (mag > 1.0) {
      gl_FragColor.a = 0.0;
    	return;
    }
}
