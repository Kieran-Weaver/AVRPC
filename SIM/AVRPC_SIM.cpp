#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include "AVRPC_SIM.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <string>
#include <fstream>

#define SCALE 6

using namespace gl;

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
	this->window = glfwCreateWindow(128 * SCALE, 160 * SCALE, "AVRPC Simulator", nullptr, nullptr);
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
	glBindTexture(GL_TEXTURE_2D, this->texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
}

void AVRPC::Draw(const std::vector<uint32_t>& pixels, uint16_t w, uint16_t h) {
	this->w = w;
	this->h = h;

	glUseProgram(this->PROG);
	glBindTexture(GL_TEXTURE_2D, this->texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->w, this->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels.data());
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
	
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

AVRPC::~AVRPC(){
	glDeleteTextures(1, &(this->texID));
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

AVRPC::operator bool(){
	return !glfwWindowShouldClose(this->window);
}
