#include "surface.h"

Surface::Surface() {
}

Surface::~Surface() {
}

void Surface::load(const char* filename) {
	//load the image in the surface
	SDL_Surface *imagen = SDL_LoadBMP(filename);
	image = imagen;
}

void Surface::destroy() {
	SDL_FreeSurface(image);
}

void Surface::blit(SDL_Surface* destination, const int x, const int y) {
	//load a destination SDL_Rect
	SDL_Rect destino;
	destino.x = x;
	destino.y = y;
	destino.w = image->w;
	destino.h = image->h;

	//Blitting
	SDL_BlitSurface(image, NULL, destination, &destino);
}

void Surface::blit(SDL_Surface* destination, const int x, const int y, const SDL_Rect& clip) {

}