#pragma once

#include <string>

class config_escenario {

	private:
		std::string nombre;
		int tam_x,tam_y;
		//protagonista-->ver organizacion
		//entidades-->ver organizacion

	public:
		//Constructor
		config_escenario(std::string nombre_inicial, int tam_x_inicial, int tam_y_inicial);

		//Getters
		std::string get_nombre() { return nombre;}

		int get_tam_x() { return tam_x;}

		int get_tam_y() { return tam_y;}

};