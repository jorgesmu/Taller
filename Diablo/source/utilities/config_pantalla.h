#pragma once

#include <string>

class config_pantalla {

	private:
		int ancho,alto;

	public:
		//Constructor
		config_pantalla(int ancho_i, int alto_i);

		//Getters
		int get_ancho() { return ancho;}

		int get_alto() { return alto;}

		void set_alto(int nuevaMedida){
			alto = nuevaMedida;
		}
		void set_ancho(int nuevaMedida){
			ancho = nuevaMedida;
		}

};