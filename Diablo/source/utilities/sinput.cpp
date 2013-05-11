#include <iostream>
#include <string>
#include <SDL.h>

#include "aux_func.h"
#include "timer.h"
#include "../display/resman.h"

#include "sinput.h"

SInput::SInput(){
	rm = NULL;
	showing = false;
	t = 0;
}

SInput::~SInput() {
}

void SInput::init(ResMan* rm, int font_size, int max_size, const SDL_Color& color) {
	this->rm = rm;
	this->font_size = font_size;
	this->max_input_size = max_size;
	this->color = color;
	indicator.start();

	// Cosas de SDL
	// Enable Unicode
	SDL_EnableUNICODE(SDL_ENABLE); 
	// Para mantener la tecla apretada y que mande un keydown
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL/3);

}

const std::string& SInput::getText() const {
	return entered;
}

int SInput::handleInput(SDL_Event &event) {
    // Si no se está mostrando, no hacemos nada
    if(!isOpen()){ return SInput::RES_NOTHING; }
    // Go
    int to_return = SInput::RES_NOTHING;
    if(event.type == SDL_KEYDOWN && (t != (char)event.key.keysym.unicode || lifted.getTicks() > 100)) {
 
        if(event.key.keysym.unicode >= 32 && event.key.keysym.unicode <= 125) {
            // Append the character
			if(str.length() <= max_input_size) {
				str += (char)event.key.keysym.unicode;
			}
        }else if( event.key.keysym.sym == SDLK_RETURN && str != "") {
            // Aca procesamos el ENTER (solos si el string no está vacío)
            to_return = SInput::RES_ENTER;
            entered = str;
            str = "";
        }else if( event.key.keysym.sym == SDLK_ESCAPE){
            str = "";
            to_return = SInput::RES_CLOSE;
        }

        // Procesamos el backspace
        if(event.key.keysym.sym == SDLK_BACKSPACE && str.length() != 0) {
            //Remove a character from the end
            str.erase(str.length()-1);
        }

		// Para repeticiones
        lifted.start();
        t = (char)event.key.keysym.unicode;

    }
	// Devolvemos
    return to_return;
}


void SInput::show(SDL_Surface* dest, int x, int y){
	// Si no esta abierto, no hacemos nada
    if(!isOpen()) return;
    // Caso contrario mostramos (agregando o no el indicador)
	if(append_indicator) {
		rm->getFont(font_size)->blit(dest, x, y, str+std::string("_"), color);
	}else{
		if(str.size() > 0) {
			rm->getFont(font_size)->blit(dest, x, y, str, color);
		}
	}

	// Para mostrar el indicador
	if(indicator.getTicks() > 500) {
		append_indicator = !append_indicator;
		indicator.start();
	}
}


bool SInput::isOpen() const {
	return showing;
}

void SInput::open() {
	showing = true;
	indicator.start();
}

void SInput::hide() {
	str.clear();
	showing = false;
}