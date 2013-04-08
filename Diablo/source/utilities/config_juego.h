#pragma once

#include <string>
#include "config_pantalla.h"
#include "config_entidad.h"
#include "config_general.h"
#include "config_escenario.h"
#include "config_juego.h"

using namespace std;
class config_juego {
	

	private:
		config_pantalla pantalla;
		vector <config_entidad> entidades;
		config_general config;
		vector <config_escenario> escenarios;

	public:
		//Constructor

		config_juego();

		//Getters
		config_pantalla* get_pantalla() { return &pantalla; }

		vector <config_entidad> get_entidades() { return entidades; }

		config_general get_configuracion() { return config; }
		
		vector <config_escenario> get_escenarios() { return escenarios; }
	
		//Setters
		void set_pantalla(config_pantalla newValue) { pantalla = newValue; }
		
		void set_entidades(vector <config_entidad> newValue) { entidades = newValue; }

		void set_configuracion(config_general newValue) { config = newValue; }
		
		void set_escenarios(vector <config_escenario> newValue) { escenarios = newValue; }
		

};