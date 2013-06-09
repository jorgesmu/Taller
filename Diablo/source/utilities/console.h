#pragma once

#include "SDL.h"
#include "timer.h"
#include <string>
#include <deque>

class ResMan;

class Console {
    public:
    Timer timeout; // Timer para los mensajes
    SDL_Color textColor; // Color 
    int max_rows; // Maxima cantidad de mensajes a mostrar
    std::deque<std::string> data_rows; // Estructura donde se guardan los mensajes
	ResMan* rm; // Link al resource manager
	int font_h; // Altura del font
	int x, y; // Coordenadas de blitteo
	int font_size; // Tamaño del font (Font::SIZE_*)

	public:
    Console();
	void init(ResMan* rm, int rows, int x, int y, int font_size, const SDL_Color& color);
    void log(const std::string& str);
    void update();
    void show(SDL_Surface *where);
    void clean();
};