#include <iostream>
#include <cassert>

#include "surface.h"
#include "../utilities/logErrores.h"
#include <sstream>

Surface::Surface() {
	image = NULL;
}

void Surface::nuevoSurfaceConfigurado(const int alto , const int ancho , const SDL_VideoInfo* vi ,
									const int colorKey) {
	if (this -> image != NULL) {
		SDL_FreeSurface(image);
		this -> image = NULL;
	}
	this -> image = SDL_CreateRGBSurface (SDL_HWSURFACE, ancho , alto , 
					vi->vfmt->BitsPerPixel , vi->vfmt->Rmask, vi->vfmt->Gmask,
					vi->vfmt->Bmask , vi->vfmt->Amask);
	Uint32 ckey = SDL_MapRGB(image -> format , 0, colorKey, 0);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckey);

	if (this -> imageGris != NULL) {
		SDL_FreeSurface(imageGris);
		this -> imageGris = NULL;
	}
	imageGris = SDL_DisplayFormat(image);
	Uint32 ckeyGris = SDL_MapRGB(imageGris -> format , 0, Surface::BMP_TRANSPARENCIA, 0);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckeyGris);

	//convierte a escala de grises
	for (int y=0; y < imageGris->h; y++){
		for (int x=0; x < imageGris->w; x++){
			Uint8 r,g,b,a;
			Uint32 pixel = getpixel(imageGris, x, y);
			SDL_GetRGB(pixel, imageGris->format, &r, &g, &b);
			r = g = b = (r+g+b)/3;
			pixel = SDL_MapRGB(imageGris->format, r, g, b);
			putpixel(imageGris, x, y, pixel);
		}
	}
}

void Surface::nuevoSurfaceConfigurado(const int alto , const int ancho , const SDL_VideoInfo* vi) {
	this -> image = SDL_CreateRGBSurface (SDL_HWSURFACE, ancho , alto , 
					vi->vfmt->BitsPerPixel , vi->vfmt->Rmask, vi->vfmt->Gmask,
					vi->vfmt->Bmask , vi->vfmt->Amask);
	Uint32 ckey = SDL_MapRGB(image -> format , 0,Surface::BMP_TRANSPARENCIA , 0);
	SDL_SetColorKey(image , SDL_SRCCOLORKEY , ckey);

	if (this -> imageGris != NULL) {
		SDL_FreeSurface(imageGris);
		this -> imageGris = NULL;
	}
	imageGris = SDL_DisplayFormat(image);
	Uint32 ckeyGris = SDL_MapRGB(imageGris -> format , 0, Surface::BMP_TRANSPARENCIA, 0);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckeyGris);

	//convierte a escala de grises
	for (int y=0; y < imageGris->h; y++){
		for (int x=0; x < imageGris->w; x++){
			Uint8 r,g,b,a;
			Uint32 pixel = getpixel(imageGris, x, y);
			SDL_GetRGB(pixel, imageGris->format, &r, &g, &b);
			r = g = b = (r+g+b)/3;
			pixel = SDL_MapRGB(imageGris->format, r, g, b);
			putpixel(imageGris, x, y, pixel);
		}
	}
}

Surface::~Surface() {
	if(image != NULL) {
		SDL_FreeSurface(image);
	}
	this -> image = NULL;
	if(imageGris != NULL) {
		SDL_FreeSurface(imageGris);
	}
}

bool Surface::load(const std::string& filename) {
	// Por si las moscas
	if (this -> image != NULL) {
		SDL_FreeSurface(image);
		this -> image = NULL;
	}
	// Load the image in the surface
	SDL_Surface *temp_surf = IMG_Load(filename.c_str());
	// Check for errors
	if(temp_surf != NULL) {
		// Optimized format
		image = SDL_DisplayFormat(temp_surf);
		SDL_FreeSurface(temp_surf);
		// Apply color key
		Uint32 ckey = SDL_MapRGB(image -> format , 0, Surface::BMP_TRANSPARENCIA, 0);
		SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckey);

		imageGris = SDL_DisplayFormat(image);
		Uint32 ckeyGris = SDL_MapRGB(imageGris -> format , 0, Surface::BMP_TRANSPARENCIA, 0);
		SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckeyGris);

		//convierte a escala de grises
		for (int y=0; y < imageGris->h; y++){
			for (int x=0; x < imageGris->w; x++){
				Uint8 r,g,b,a;
				Uint32 pixel = getpixel(imageGris, x, y);
				SDL_GetRGB(pixel, imageGris->format, &r, &g, &b);
				r = g = b = (r+g+b)/3;
				pixel = SDL_MapRGB(imageGris->format, r, g, b);
				putpixel(imageGris, x, y, pixel);
			}
		}
		return true;
	}else{
		std::stringstream ss;
		ss << "Error loading " << filename << ": " << IMG_GetError() << "\n";
		err_log.escribir(ss.str());
		return false;
	}
}

bool Surface::load(const std::string& filename, const int colorKey) {
	// Load the image in the surface
	SDL_Surface *temp_surf = IMG_Load(filename.c_str());
	// Check for errors
	if(temp_surf != NULL) {
		// Optimized format
		image = SDL_DisplayFormat(temp_surf);
		SDL_FreeSurface(temp_surf);
		// Apply color key
		Uint32 ckey = SDL_MapRGB(image -> format , 0, colorKey, 0);
		SDL_SetColorKey(image, SDL_SRCCOLORKEY , ckey);

		imageGris = SDL_DisplayFormat(image);
		Uint32 ckeyGris = SDL_MapRGB(imageGris -> format , 0, colorKey, 0);
		SDL_SetColorKey(image, SDL_SRCCOLORKEY, ckeyGris);

		//convierte a escala de grises
		for (int y=0; y < imageGris->h; y++){
			for (int x=0; x < imageGris->w; x++){
				Uint8 r,g,b,a;
				Uint32 pixel = getpixel(imageGris, x, y);
				SDL_GetRGB(pixel, imageGris->format, &r, &g, &b);
				r = g = b = (r+g+b)/3;
				pixel = SDL_MapRGB(imageGris->format, r, g, b);
				putpixel(imageGris, x, y, pixel);
			}
		}

		return true;
	}else{
		std::stringstream ss;
		ss << "Error loading " << filename << ": " << IMG_GetError() << "\n";
		err_log.escribir(ss.str());
		return false;
	}
}


Uint32 Surface::getpixel(SDL_Surface *surface, int x, int y){
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;
	}
}

void Surface::putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel){
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

void Surface::destroy() {
	if(image != NULL) {
		SDL_FreeSurface(image);
		this -> image = NULL;
	}
	if(imageGris != NULL) {
		SDL_FreeSurface(imageGris);
		this -> imageGris = NULL;
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


void Surface::blitGris(SDL_Surface* destination, int x, int y) {
	//load a destination SDL_Rect
	SDL_Rect destino;
	destino.x = x;
	destino.y = y;

	//Blitting
	SDL_BlitSurface(imageGris, NULL, destination, &destino);
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


void Surface::blitGris(SDL_Surface* destination, int x, int y, SDL_Rect& clip) {
	assert(imageGris != NULL);
	//load destination SDL_Rect
	SDL_Rect destino;
	destino.x = x;
	destino.y = y;

	//Blitting
	SDL_BlitSurface(imageGris, &clip, destination, &destino);
}

int Surface::height() const {
	assert(image != NULL);
	return image->h;
}

int Surface::width() const {
	assert(image != NULL);
	return image->w;
}

SDL_Surface* Surface::getSDL_Surface() {
	return this -> image;
}

SDL_Surface* Surface::getSDL_SurfaceGris() {
	return this -> imageGris;
}

void Surface::setSurface(SDL_Surface* nuevaSurface) {
	SDL_FreeSurface(this->image);
	this->image=nuevaSurface;
}

