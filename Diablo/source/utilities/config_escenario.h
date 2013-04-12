#pragma once

#include <string>
#include <vector>
#include "config_entidad_en_juego.h"
class config_escenario {

	private:
		std::string nombre;
		int tam_x,tam_y;
		vector <config_entidad_en_juego> entidades;
		vector <config_entidad_en_juego> protagonistas;
		bool completoNombre, completoTamX, completoTamY, completoEntidades, completoProtagonista;
	public:
		//Constructor
		config_escenario(std::string nombre_inicial, int tam_x_inicial, int tam_y_inicial);

		//Getters
		string get_nombre() { return nombre;}

		int get_tam_x() { return tam_x;}

		int get_tam_y() { return tam_y;}
		
		vector<config_entidad_en_juego> get_entidades() { return entidades;}
		
		vector<config_entidad_en_juego> get_protagonitas() { return protagonistas;}

		config_entidad_en_juego get_protagonista() { 
			if ( protagonistas.size() > 0 ) {
				protagonistas[0];
			}
			config_entidad_en_juego protagonistaVacio("",-1,-1);
			
			return protagonistaVacio ;
		}
		//Gets de los flags de control
		bool get_completo_nombre() { return completoNombre;}
		
		bool get_completo_tamX() { return completoTamX;}
		
		bool get_completo_tamY() { return completoTamY;}
		
		bool get_completo_protagonista() { return completoProtagonista;}
		
		bool get_completo_entidades() { return completoEntidades;}
		
		//Setters
		void set_nombre(string newValue) { 
			nombre = newValue ;
			completoNombre = true;
		}

		void set_tam_x(int newValue) {
			tam_x = newValue;
			completoTamX = true;
		}

		void set_tam_y(int newValue) { 
			tam_y = newValue;
			completoTamY = true;
		}
		
		void agregar_entidad(config_entidad_en_juego unaEntidad) { 
			entidades.push_back(unaEntidad);
			completoEntidades = true;
		}

		void agregar_protagonista(config_entidad_en_juego unProtagonista) { 
			protagonistas.push_back(unProtagonista);
			completoProtagonista = true;
		}	
};
