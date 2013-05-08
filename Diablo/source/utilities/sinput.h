#pragma once

#include <string>
#include <SDL.h>
#include "timer.h"

// Fwd Decl.
class ResMan;

// Key press interpreter
class SInput {
	private:
    //The storage string
    std::string str, entered;
	// Estas variables son para poder procesar repeticiones
	// Y para mostrar el indicator
    Timer lifted, indicator;
	bool append_indicator;
    char t;
	// Se está mostrando o no
    bool showing;
	// Maxima longitud
    int max_input_size;
	// Tamaño y color del font
	int font_size;
	SDL_Color color;
	// Link con el resource manager
	ResMan* rm;
	public:
    // Ctor y Dtor
    SInput();
	~SInput();
	// Init
	void init(ResMan* rm, int font_size, int max_size, const SDL_Color& color);
    // Maneja el input
	// Devuelve: 
	const static int RES_ENTER = 0; // Cuando el usuario aprieta enter (obtener el string con getText())
	const static int RES_CLOSE = 1; // Cuando el usuario cierra (via ESC)
	const static int RES_NOTHING = 2; // Cuando no se está procesando input
    int handleInput(SDL_Event &event);
	// Devuelve el texto ingresado
	const std::string& getText() const;
    // Muestra el input en la pantalla
    void show(SDL_Surface* dest, int x, int y);
	// Devuelve si está abierto o no el input
	bool isOpen() const;
	// Abre el input
	void open();
	// Cierra
	void hide();
};
