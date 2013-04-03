#pragma once

#include <string>

class config_general {

	private:
		int vel_personaje,margen_scroll;

	public:
		//Constructor
		config_general(int vel_personaje_i, int margen_scroll_i);

		//Getters
		int get_vel_personaje() { return vel_personaje;}

		int get_margen_scroll() { return margen_scroll;}

		//Setters
		void set_vel_personaje(int newValue) { vel_personaje = newValue;}

		void set_margen_scroll(int newValue) { margen_scroll = newValue;}


};