varying vec3 vPos;
varying vec3 vUV;

uniform sampler2D uColorTexture;

void main() {

	vec4 t = vec4(vUV, 1.0);
	vec4 c = texture2D(uColorTexture, t.xy);
	gl_FragColor = c;
	
}
