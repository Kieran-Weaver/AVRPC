#version 330
in vec2 Frag_UV;
out vec4 outcolor;

uniform sampler2DRect Texture;
// inverted, sleep, idle, flip
uniform uvec4 flags;

void main(){
	vec2 texcoord = Frag_UV.st;
	
	// Y flip
	if ((flags.w & 2u) >= 1u) {
		texcoord.y = 1.0 - texcoord.y;
	}
	
	// X flip
	if ((flags.w & 1u) >= 1u) {
		texcoord.x = 1.0 - texcoord.x;
	}
	
	vec4 texel = texture(Texture, texcoord);

	// Sleep
	if (flags.y >= 1u) {
		outcolor = vec4(0.f);
	} else {
		// Idle
		if (flags.z >= 1u) {
			texel.rgb = floor(texel.rgb + vec3(0.5f));
		}
		// Inverted
		if (flags.x >= 1u) {
			texel.rgb = vec3(1.0f) - texel.rgb;
		}
		
		outcolor = clamp(texel, 0.f, 1.f);
	}
}
