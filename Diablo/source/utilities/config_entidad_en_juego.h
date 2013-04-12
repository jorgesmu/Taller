#pragma once

#include <string>
using namespace std;
class config_entidad_en_juego {

	private:
		string nombre;
		int pos_x,pos_y;
		bool completoNombre, completoPosX, completoPosY;
	public:
		//Constructor
		config_entidad_en_juego(string nombreValue, int pos_x_value, int pos_y_value);

		//Getters
		string get_nombre() { return nombre;}

		int get_pos_x() { return pos_x;}

		int get_pos_y() { return pos_y;}

		//Gets de los flags de control
		bool get_completo_nombre() { return completoNombre;}
		
		bool get_completo_posX() { return completoPosX;}
		
		bool get_completo_posY() { return completoPosY;}

		//Setters
		void set_nombre(string newValue) { 
			nombre = newValue;
			completoNombre = true;
		}

		void set_pos_x(int newValue) {
			pos_x = newValue;
			completoPosX = true;
		}

		void set_pos_y(int newValue) { 
			pos_y = newValue;
			completoPosY = true;
		}
};