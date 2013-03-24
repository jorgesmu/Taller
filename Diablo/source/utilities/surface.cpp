#include <iostream>
#include <cassert>

#include "surface.h"

Surface::Surface() {
	image = NULL;
}

Surface::~Surface() {
	// Nada
}

bool Surface::load(const std::string& filename) {
	// Load the image in the surface
	SDL_Surface *temp_surf = SDL_LoadBMP(filename.c_str());
	// Check for errors
	if(temp_surf != NULL) {
		// Optimized format
		image = SDL_DisplayFormat(temp_surf);
		SDL_FreeSurface(temp_surf);
		// Apply color key
		Uint32 ckey = SDL_MapRGB(image->format, 0, 0xFF, 0);
		SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckey);
		return true;
	}else{
		std::cerr << "Error loading " << filename << "\n";
		return false;
	}
}

void Surface::destroy() {
	if(image != NULL) {
		SDL_FreeSurface(image);
	}
}

void Surface::blit(SDL_Surface* destination, int x, int y) {
	//load a destination SDL_Rect
	SDL_Rect destino;
	destino.x = x;
	destino.y = y;

	//Blitting
	SDL_BlitSurface(image, NULL, destination, &destino);
}

void Surface::blit(SDL_Surface* destination, int x, int y, SDL_Rect& clip) {
	assert(image != NULL);
	//load destination SDL_Rect
	SDL_Rect destino;
	destino.x = x;
	destino.y = y;

	//Blitting
	SDL_BlitSurface(image, &clip, destination, &destino);
}

int Surface::height() const {
	assert(image != NULL);
	return image->h;
}

int Surface::width() const {
	assert(image != NULL);
	return image->w;
}