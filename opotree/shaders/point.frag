#version 120

//#define SQUARE_POINT_SHAPE
//#define CIRCLE_POINT_SHAPE

//varying vec3 vViewPosition;
//varying vec3 vWorldPosition;
//varying float vLinearDepth;
varying vec3 vColor;
//varying float vRadius;

void main() {

	vec3 lightDir = vec3(0.577, 0.577, 0.577);

  	vec3 N;
    N.xy = gl_PointCoord* 2.0 - vec2(1.0);    
    float mag = dot(N.xy, N.xy);
    //if (mag > 1.0) discard;   // kill pixels outside circle
    if(mag > 1.0) {
	gl_FragColor.a = 0.0;
	return;
    }
    N.z = sqrt(1.0-mag);

    // calculate lighting
    float diffuse = max(0.0, dot(lightDir, N));

    gl_FragColor = vec4(vColor, 1); // * diffuse;  	
}
