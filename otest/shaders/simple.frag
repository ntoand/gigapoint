#version 120

void main() {
  
  	vec3 lightDir = vec3(0.577, 0.577, 0.577);

  	vec3 N;
    N.xy = gl_PointCoord* 2.0 - vec2(1.0);    
    float mag = dot(N.xy, N.xy);
    if (mag > 1.0) {
    	//gl_FragColor = gl_Color;
      gl_FragColor.a = 0.0;
    	return;
     	//discard;   // kill pixels outside circle
    }
    N.z = sqrt(1.0-mag);

    // calculate lighting
    //float diffuse = max(0.0, dot(lightDir, N));

    //gl_FragColor = gl_Color * diffuse;
    //gl_FragColor.a = 1.0;
    gl_FragColor = gl_Color;
}
