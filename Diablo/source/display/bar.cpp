#include "bar.h"
#include "../utilities/aux_func.h"

void Bar::setSize(int w, int h) {
	this->w = w;
	this->h = h;
}

void Bar::setPerc(int perc) {
	if(perc > 100) perc = 100;
	if(perc < 0) perc = 0;
	this->perc = perc;
}

void Bar::setColor(const SDL_Color& color) {
	this->color = color;
}

void Bar::blit(SDL_Surface* dest, int x, int y) {
	SDL_Rect rect;
	// Fondo
	rect.x = x-1; rect.y = y-1; rect.w = this->w+3; rect.h = this->h+3;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, COLOR::BLACK.r, COLOR::BLACK.g, COLOR::BLACK.b));
	// Barra
	rect.x = x+1; rect.y = y+1; rect.w = int((this->w-1)*float(this->perc)/100.0f); rect.h = this->h-1;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, this->color.r, this->color.g, this->color.b));
}