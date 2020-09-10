#ifndef AVRPC_SIM_HPP
#define AVRPC_SIM_HPP
#include "PDQ_ST7735.h"
#include <cstdint>
struct GLFWwindow;
class AVRPC{
public:
	AVRPC();
	~AVRPC();
	void Draw(PDQ_ST7735& sim);
	operator bool();
private:
	uint32_t texID;
	uint32_t VAO;
	uint32_t VBO;
	uint32_t VS;
	uint32_t FS;
	uint32_t PROG;
	uint16_t w;
	uint16_t h;
	GLFWwindow* window;
};
#endif
