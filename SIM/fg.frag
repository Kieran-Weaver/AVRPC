#version 330
in vec2 Frag_UV;
out vec4 outcolor;

uniform sampler2D Texture;
// inverted, sleep, idle, scrolling
uniform uvec4 flags;

void main(){
	vec4 texel = texture(Texture, Frag_UV.st);
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
