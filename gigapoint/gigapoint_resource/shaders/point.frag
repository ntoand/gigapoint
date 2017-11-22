varying vec3 vColor;
#if defined FILTER_EDL
varying float vDepth;
#endif

#if defined SPHERE_POINT_SHAPE
varying vec3 vPosEye;
#endif

void main() {

  	gl_FragColor = vec4(vColor,1.0);
#if defined FILTER_EDL
  	gl_FragColor.a = vDepth;
#endif

#if defined CIRCLE_POINT_SHAPE
    vec3 N;
    N.xy = gl_PointCoord* 2.0 - vec2(1.0);    
    float R = dot(N.xy, N.xy);
    if (R > 1.0) {
        discard;
    }
#endif

#if defined SPHERE_POINT_SHAPE
	vec3 N;
    N.xy = gl_PointCoord* 2.0 - vec2(1.0);    
    float R = dot(N.xy, N.xy);
    if (R > 1.0) {
        discard;
    }
    
    //lighting
    N.z = sqrt(1.0-R);
    vec3 lightDir = normalize(-vPosEye);
    float diffuse = max(0.0, dot(lightDir, N));
    float shininess = 200; //Size of highlight
    vec3 specolour = vec3(1.0, 1.0, 1.0);   //Color of light
    vec3 halfVector = normalize(vec3(0.0, 0.0, 1.0) + lightDir);
    float NdotHV = max(dot(N, halfVector), 0.0);
    vec3 specular = specolour * pow(NdotHV, shininess);
    vec4 colour = vec4(gl_FragColor.rgb * diffuse + specular, 1.0); //alpha
    gl_FragColor = colour;
    
#endif

}
