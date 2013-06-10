#include <iostream>
#include <string>
#include <SDL.h>

#include "../utilities/aux_func.h"
#include "../utilities/timer.h"
#include "../display/resman.h"

#include "boton.h"

Boton::Boton(){
}

Boton::~Boton() {
}

void Boton::init(ResMan* rm, int x, int y, const std::string& res_name) {
	this->rm = rm;
	this->x = x;
	this->y = y;
	this->res_name = res_name;
	this->h = rm->getRes(res_name)->height();
	this->w = rm->getRes(res_name)->width();
}

bool Boton::handleInput(SDL_Event &event) {
	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		if(		event.button.x < x + w &&
				event.button.x > x &&
				event.button.y > y &&
				event.button.y < y + h)
		{
			return true;
		}
	}
	return false;
}


void Boton::show(SDL_Surface* dest){
	rm->getRes(res_name)->blit(dest, x, y);
}