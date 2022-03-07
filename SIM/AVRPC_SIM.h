#ifndef AVRPC_SIM_HPP
#define AVRPC_SIM_HPP
#include <cstdint>
#include <vector>

struct nk_context;
struct nk_glfw;
struct TFT_State;
struct GLFWwindow;

class AVRPC{
public:
	AVRPC();
	~AVRPC();
	struct nk_context* ctx();
	void Draw(const std::vector<uint32_t>& pixels, const TFT_State& state);
	operator bool();
private:
	uint32_t texID;
	uint32_t VAO;
	uint32_t VBO;
	uint32_t IBO;
	uint32_t VS;
	uint32_t FS;
	uint32_t PROG;
	uint32_t PROG_flags;
	uint16_t w;
	uint16_t h;
	GLFWwindow* window;
	struct nk_glfw* ctx_;
	// Vertex Buffers: X, Y, U, V
	std::array<float, 48> scroll = {
	// Top Quad
		-1.0f, 1.0f,  0.f, 1.f,	// Top left
		-1.0f, -1.0f, 0.f, 0.f,	// Bottom left
		1.0f, -1.0f,  1.f, 0.f,	// Bottom right
		1.0f, 1.0f,   1.f, 1.f,	// Top right
	// Middle Quad + 16
		-1.0f, 1.0f,  0.f, 1.f,	// Top left
		-1.0f, -1.0f, 0.f, 0.f,	// Bottom left
		1.0f, -1.0f,  1.f, 0.f,	// Bottom right
		1.0f, 1.0f,   1.f, 1.f,	// Top right
	// Bottom Quad + 32
		-1.0f, 1.0f,  0.f, 1.f,	// Top left
		-1.0f, -1.0f, 0.f, 0.f,	// Bottom left
		1.0f, -1.0f,  1.f, 0.f,	// Bottom right
		1.0f, 1.0f,   1.f, 1.f	// Top right
	};
	const std::array<float, 16> noscroll = {
		-1.0f, 1.0f,  0.f, 1.f,	// Top left
		-1.0f, -1.0f, 0.f, 0.f,	// Bottom left
		1.0f, -1.0f,  1.f, 0.f,	// Bottom right
		1.0f, 1.0f,   1.f, 1.f	// Top right
	};
};

#endif
