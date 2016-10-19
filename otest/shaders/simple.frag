#version 120

varying vec3 vcolor;

void main() {
	vec2 cxy = gl_PointCoord * 2.0 - vec2(1.0);
        float r = dot(cxy, cxy);
        if (r > 1.4)
                discard;
  	gl_FragColor = vec4(vcolor, 1.0);
}
