#define NEIGHBOUR_COUNT 4

varying vec3 vUV;

uniform sampler2D uColorTexture;
uniform float uScreenWidth;
uniform float uScreenHeight;
uniform vec2 uNeighbours[NEIGHBOUR_COUNT];
uniform float uEdlStrength;
uniform float uRadius;
uniform float uOpacity;

const float infinity = 1.0 / 0.0;

float response(float depth){
	vec2 uvRadius = uRadius / vec2(uScreenWidth, uScreenHeight);
	
	float sum = 0.0;
	
	for(int i = 0; i < NEIGHBOUR_COUNT; i++){
		vec2 uvNeighbor = vUV.xy + uvRadius * uNeighbours[i];
		
		float neighbourDepth = texture2D(uColorTexture, uvNeighbor).a;
		
		if(neighbourDepth == 0.0){
			neighbourDepth = infinity;
		}
		
		sum += max(0.0, depth - neighbourDepth);
	}
	
	return sum / float(NEIGHBOUR_COUNT);
}

void main() {

	vec4 color = texture2D(uColorTexture, vUV.xy);
	
	float depth = color.a;
	
	if(depth == 0.0){
		depth = infinity;
	}
	
	float res = response(depth);
	float shade = exp(-res * 300 * uEdlStrength);
	
	if(color.a == 0.0 && res == 0.0){
		discard;
	}

	gl_FragColor = vec4(color.rgb * shade, uOpacity);
	
	/*
	vec4 t = vec4(vUV, 1.0);
	vec4 c = texture2D(uColorTexture, t.xy);
	gl_FragColor = c;
	*/
}
