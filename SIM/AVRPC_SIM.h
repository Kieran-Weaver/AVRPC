#ifndef AVRPC_SIM_HPP
#define AVRPC_SIM_HPP
#include <cstdint>
#include <vector>
struct GLFWwindow;
class AVRPC{
public:
	AVRPC();
	~AVRPC();
	void Draw(const std::vector<uint32_t>& pixels, uint16_t w, uint16_t h);
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
