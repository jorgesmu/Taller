#include "aux_func.h"

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