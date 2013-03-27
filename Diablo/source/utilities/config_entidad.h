#pragma once

#include <string>

class config_entidad {

	private:
		std::string nombre,path_imagen;
		int ancho_base,alto_base,pixel_ref_x,pixel_ref_y,fps,delay;


	public:
		//Constructor
		//i: inicial
		config_entidad(std::string nombre_i, int path_imagen_i, int ancho_base_i, int alto_base_i, int pixel_ref_x_i, int pixel_ref_y_i, int fps_i, int delay_i);

		//Getters
		std::string get_nombre() { return nombre;}

		std::string get_path_imagen() { return path_imagen;}

		int get_ancho_base() { return ancho_base;}

		int get_alto_base() { return alto_base;}

		int get_pixel_ref_x() { return pixel_ref_x;}

		int get_pixel_ref_y() { return pixel_ref_y;}

		int get_fps() { return fps;}

		int get_delay() { return delay;}

		

};