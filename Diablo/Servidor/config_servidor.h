#pragma once

#include <string>
using namespace std;
class config_servidor {

	private:
		int puerto;
		bool completoPuerto;
	public:
		//Constructor
		config_servidor(int puerto_destino);

		//Getters
		int get_puerto(); 

		bool get_completo_puerto(); 

		//Setters
		void set_puerto(int newValue); 
};