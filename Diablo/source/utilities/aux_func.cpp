#include "aux_func.h"
#include <fstream>

SDL_Rect makeRect(int x, int y) {
	SDL_Rect r;
	r.x = x;
	r.y = y;
	return r;
}

SDL_Rect makeRect(int x, int y, int w, int h) {
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

int intRand(int min, int max) {
    return min + (rand() % (int)(max - min + 1));
}

float fRand(float v1, float v2) {
    return v1 + (v2-v1)*((float)rand())/((float)RAND_MAX);
}

size_t fileSize(const std::string& file) {
	std::ifstream f(file, std::ios_base::app | std::ios_base::binary | std::ios_base::in);
	if(f.bad())
		return 0;

	size_t r = f.tellg();
	f.close();
	return r;
}