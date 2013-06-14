#pragma once

#include "SDL.h"

class Bar {
	private:
		int w, h;
		int perc;
		SDL_Color color;
	public:
		void setSize(int w, int h);
		void setColor(const SDL_Color& color);
		void setPerc(int perc);
		void blit(SDL_Surface* dest, int x, int y);
};