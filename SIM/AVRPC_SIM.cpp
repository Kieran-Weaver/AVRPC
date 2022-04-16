#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include "AVRPC_SIM.h"
#include "TFT_ILI9163.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace gl;

#define MAX_VERTEX_BUFFER 256 * 1024
#define MAX_ELEMENT_BUFFER 512 * 1024
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "../nuklear/nuklear.h"

#define glfwGetWindowUserPointer(s) (struct nk_glfw*)glfwGetWindowUserPointer(s)
#include "nuklear_glfw_gl3.h"
#undef glfwGetWindowUserPointer

#define SCALE 6

std::string readWholeFile(const std::string& filename){
	std::ifstream infile(filename,std::ios::in | std::ios::binary | std::ios::ate);
	std::string contents;
	if (infile){
		int size = infile.tellg();
		contents.resize(size);
		infile.seekg(0, std::ios::beg);
		infile.read(&contents[0],size);
		infile.close();
	}
	return contents;
}

template<typename T, int N>
constexpr std::array<T, N*6> genBufImpl(void){
	constexpr std::array<uint8_t, 6> vertices = { 0, 1, 2, 0, 2, 3 };
	std::array<T, N*6> ibo;

	for (int i = 0; i < N; i++){
		for (int j = 0; j < 6; j++){
			ibo[(i * 6) + j] = vertices[j] + (i * 4);
		}
	}

	return ibo;
}

AVRPC::AVRPC(){
	const std::array<uint8_t, 18> ibo = genBufImpl<uint8_t, 3>();
	
	if (glfwInit() != GLFW_TRUE){
		std::exit(0);
	}
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	this->window = glfwCreateWindow(128 * (SCALE * 1.5), 160 * SCALE, "AVRPC Simulator", nullptr, nullptr);
	if (!this->window){
		glfwTerminate();
		std::exit(0);
	}
	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);
	
	glbinding::initialize(glfwGetProcAddress);
	glGenVertexArrays(1, &(this->VAO));
	glBindVertexArray(this->VAO);

	glGenTextures(1, &(this->texID));
	glBindTexture(GL_TEXTURE_RECTANGLE, this->texID);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenBuffers(1, &(this->VBO));
	glGenBuffers(1, &(this->IBO));
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
	
	glBufferData(GL_ARRAY_BUFFER, noscroll.size()*sizeof(float), noscroll.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo.size(), ibo.data(), GL_STATIC_DRAW);

	std::string vtxSource = readWholeFile("vtx.vert");
	this->VS = glCreateShader(GL_VERTEX_SHADER);
	const char* vsc = vtxSource.c_str();
	glShaderSource(this->VS, 1, &vsc, nullptr);
	glCompileShader(this->VS);
	
	std::string fgSource = readWholeFile("fg.frag");
	const char* fgc = fgSource.c_str();
	this->FS = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->FS, 1, &fgc, nullptr);
	glCompileShader(this->FS);
	
	this->PROG = glCreateProgram();
	glAttachShader(this->PROG, this->VS);
	glAttachShader(this->PROG, this->FS);
	glBindFragDataLocation(this->PROG, 0, "outcolor");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, 0);
	glBindAttribLocation(this->PROG, 0, "Position");
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (const void*)8);
	glBindAttribLocation(this->PROG, 1, "UV");	
	glLinkProgram(this->PROG);
	glUseProgram(this->PROG);
	glUniform1i(glGetUniformLocation(this->PROG, "Texture"), 0);
	
	this->PROG_flags = glGetUniformLocation(this->PROG, "flags");
	
	this->ctx_ = new struct nk_glfw;
	nk_glfw3_init(ctx_, this->window, NK_GLFW3_INSTALL_CALLBACKS);
	
	struct nk_font_atlas* atlas;
	nk_glfw3_font_stash_begin(ctx_, &atlas);
	nk_glfw3_font_stash_end(ctx_);
	
	glfwPollEvents();
	nk_glfw3_new_frame(ctx_);
}

struct nk_context* AVRPC::ctx(void) {
	return &(this->ctx_->ctx);
}

// Convert V coordinate (0 to 1) into Y coordinate (1 to -1)
constexpr float VtoY(float V) {
	return (V * 2.f) - 1.f;
}

void AVRPC::Draw(const std::vector<uint32_t>& pixels, const TFT_State& state) {
	this->w = state.w;
	this->h = state.h;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.f, 1.f, 1.f, 1.f);

	glUseProgram(this->PROG);
	glBindTexture(GL_TEXTURE_RECTANGLE, this->texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, this->w, this->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels.data());
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glUniform4ui(this->PROG_flags,
		state.inverted,
		state.sleep,
		state.idle,
		state.xflip * 2 + state.yflip
	);
	
	if (state.scrolling) {
		// Top Quad: Process TFA
		// TFA V coordinates
		scroll[7] = scroll[11] = float(h - state.TFA) / float(h);
		// TFA Y coordinates
		scroll[5] = scroll[9]  = VtoY(scroll[7]);
		
		// Bottom Quad: Process BFA
		// BFA V coordinates
		scroll[15 + 32] = scroll[3 + 32] = float(state.BFA) / float(h);
		// BFA Y coordinates
		scroll[1 + 32] = scroll[13 + 32] = VtoY(scroll[3 + 32]);

		// Middle Quad: Set coordinates
		scroll[1 + 16] = scroll[13 + 16] = scroll[9]; // Top Y
		scroll[5 + 16] = scroll[9 + 16] = scroll[1 + 32]; // Bottom Y
		
		// Middle Quad: Set UV
		scroll[3 + 16] = scroll[15 + 16] = float(h - state.SSA) / float(h); // Top V
		scroll[7 + 16] = scroll[11 + 16] = float(h - (state.SSA + state.VSA)) / float(h); // Bottom V
		
		glBufferData(GL_ARRAY_BUFFER, scroll.size()*sizeof(float), scroll.data(), GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, NULL);
	} else {
		glBufferData(GL_ARRAY_BUFFER, noscroll.size()*sizeof(float), noscroll.data(), GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
	}
	
	nk_glfw3_render(ctx_, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
	glfwSwapBuffers(this->window);
	glfwPollEvents();
	nk_glfw3_new_frame(ctx_);
}

AVRPC::~AVRPC(){
	glDeleteTextures(1, &(this->texID));
	nk_glfw3_shutdown(this->ctx_);
	delete this->ctx_;
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

AVRPC::operator bool(){
	return !glfwWindowShouldClose(this->window);
}
