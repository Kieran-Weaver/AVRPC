#include "gl.h"
#include "AVRPC_SIM.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <string>
#include <fstream>
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
AVRPC::AVRPC(){
	std::array<float, 16> vbo_data = {
		-1.0f, -1.0f, 0.f, 0.f,	// Bottom left
		-1.0f, 1.0f,  0.f, 1.f,	// Top left
		1.0f, -1.0f,  1.f, 0.f,	// Bottom right
		1.0f, 1.0f,   1.f, 1.f	// Top right
	};
	if (glfwInit() != GLFW_TRUE){
		std::exit(0);
	}
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	this->window = glfwCreateWindow(768, 960, "AVRPC Simulator", nullptr, nullptr);
	if (!this->window){
		glfwTerminate();
		std::exit(0);
	}
	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);
	glGenVertexArrays(1, &(this->VAO));
	glBindVertexArray(this->VAO);

	glGenTextures(1, &(this->texID));
	glBindTexture(GL_TEXTURE_2D, this->texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenBuffers(1, &(this->VBO));
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, 16*sizeof(float), vbo_data.data(), GL_STATIC_DRAW);

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

void AVRPC::Draw(TFT_ILI9163& sim){
	const std::vector<uint16_t>& pixels = sim.getPixels();
	sim.getDims(this->w, this->h);
	glUseProgram(this->PROG);
	glBindTexture(GL_TEXTURE_2D, this->texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->w, this->h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels.data());
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
