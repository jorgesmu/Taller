#pragma once

#include <string>

#include "SDL.h"
#include "SDL_image.h"
class Surface {

	public:

	static const int BMP_TRANSPARENCIA = 0x000000;

	static const int RGB_VERDE = 0x00ff00;

	static const int RGB_AZUL = 0x0000ff;

	static const int RGB_ROJO = 0xff0000;
	
	private:

	SDL_Surface* image;

	SDL_Surface* imageGris;
	
	public:
	// Constructor
	// No hace nada excepto inicializar surf en NULL
	Surface();
	// Destructor - no hace nada en ppio
	~Surface();
	// Carga y optimiza la surface del archivo filename
	// Devuelve true o false para success of fail
	bool load(const std::string& filename);
	// Devuelve true o false para success of fail
	bool load(const std::string& filename , const int colorKey);
	// Destruye la superficie y libera la memoria
	void destroy();
	// Getters para las propiedades de la imagen
	int height() const;
	int width() const;
	// Para dibujar la superficie
	// Toma como argumentos la surface de destino (aka screen) y offset coords x, y
	void blit(SDL_Surface* destination, int x, int y);
	// Idem anterior pero lleva un parametro extra para clippear (spritesheets)
	void blit(SDL_Surface* destination, int x, int y, SDL_Rect& clip);
	// Toma como argumentos la surface de destino (aka screen) y offset coords x, y
	void blitGris(SDL_Surface* destination, int x, int y);
	// Idem anterior pero lleva un parametro extra para clippear (spritesheets)
	void blitGris(SDL_Surface* destination, int x, int y, SDL_Rect& clip);
	// cambiar escala de grises
	Uint32 getpixel(SDL_Surface* surface, int x, int y);
	//cambiar escala de grises
	void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
	// Retorna el Surface contenido.
	SDL_Surface* getSDL_Surface();
	// Retorna el Surface contenido.
	SDL_Surface* getSDL_SurfaceGris();
	//Asocia la Surface con una nueva SDL_Surface
	void setSurface(SDL_Surface* nuevaSurface);
	/*
		Nota: 
			Como obtener video info:
			const SDL_VideoInfo vi* SDL_GetVideoInfo ();
	*/
	void nuevoSurfaceConfigurado(const int alto , const int ancho , const SDL_VideoInfo* vi , 
								const int colorKey);
	/*
		Nota: 
			Como obtener video info:
			const SDL_VideoInfo vi* SDL_GetVideoInfo ();
	*/
	void nuevoSurfaceConfigurado(const int alto , const int ancho , const SDL_VideoInfo* vi);
};