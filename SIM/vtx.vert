#version 330
layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
out vec2 Frag_UV;
void main(){
	Frag_UV = UV;
	gl_Position = vec4(Position, 0.f, 1.f);
}
