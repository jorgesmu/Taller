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