#version 330
in vec2 Frag_UV;
uniform sampler2D Texture;
out vec4 outcolor;
void main(){
	outcolor = texture(Texture, Frag_UV.st);
}
