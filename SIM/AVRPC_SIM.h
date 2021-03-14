#ifndef AVRPC_SIM_HPP
#define AVRPC_SIM_HPP
#include "TFT_ILI9163.h"
#include <cstdint>
struct GLFWwindow;
class AVRPC{
public:
	AVRPC();
	~AVRPC();
	void Draw(TFT_ILI9163& sim);
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
