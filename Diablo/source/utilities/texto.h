#pragma once

#include "SDL.h"
#include "SDL_ttf.h"
#include "surface.h"

class Texto {
private:
	Surface* background;
	Surface* message;
	SDL_Surface* screen;
	SDL_Event event;
	TTF_Font *font;
	SDL_Color textColor;
public:
	//Constructor de la clase
	Texto();
	//Setea el color del texto
	void setColor(int R, int G, int B);
	//Funcion de prueba
	int test();
	//Inicializa el sistema en un estado valido
	bool init(char* caption);
	//Carga los archivos correspondientes para la fuente
	bool cargarArchivos(const char* bkg,int size,const char* nombreFont);
	//Destruye las estructuras
	void destruir();
};