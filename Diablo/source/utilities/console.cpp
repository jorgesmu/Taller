#include "SDL.h"
#include "../display/font.h"
#include <string>
#include <deque>
#include <iostream>

#include "../display/resman.h"
#include "console.h"

const int CONSOLE_DELAY = 3000;

Console::Console() {
}

void Console::init(ResMan* rm, int rows, int x, int y, int font_size, const SDL_Color& color) {
	this->rm = rm;
	this->max_rows = rows;
	this->x = x;
	this->y = y;
	this->textColor = color;
	this->font_size = font_size;
	// Calculamos el height de cada linea
	font_h = rm->getFont(font_size)->getH();
}

void Console::log(const std::string& to_add){
    timeout.start();
    data_rows.push_back(to_add);
    update();
}

void Console::update(){
    if(data_rows.size() > max_rows){
        data_rows.pop_front();
    }
    if(data_rows.size() > 0 && timeout.getTicks() > CONSOLE_DELAY){
        data_rows.pop_front();
        timeout.start();
    }
}

void Console::show(SDL_Surface *where){
	// Recorremos todas las filas
	for(int i = 0;i < data_rows.size();i++){
		rm->getFont(font_size)->blit(where, x, y+(i*font_h), data_rows[i], this->textColor);
	}
}

void Console::clean(){
	data_rows.clear();
}
