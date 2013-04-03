#pragma once

#include <string>

class config_entidad {

	private:
		std::string nombre,path_imagen;
		int ancho_base,alto_base,pixel_ref_x,pixel_ref_y,fps,delay;


	public:
		//Constructor
		//i: inicial
		config_entidad(std::string nombre_i, std::string path_imagen_i, int ancho_base_i, int alto_base_i, int pixel_ref_x_i, int pixel_ref_y_i, int fps_i, int delay_i);

		//Getters
		std::string get_nombre() { return nombre;}

		std::string get_path_imagen() { return path_imagen;}

		int get_ancho_base() { return ancho_base;}

		int get_alto_base() { return alto_base;}

		int get_pixel_ref_x() { return pixel_ref_x;}

		int get_pixel_ref_y() { return pixel_ref_y;}

		int get_fps() { return fps;}

		int get_delay() { return delay;}


		//Setters
		void set_nombre(std::string newValue) { nombre = newValue;}

		void set_path_imagen(std::string newValue) { path_imagen = newValue;}
		void set_ancho_base(int newValue) { ancho_base = newValue;}

		void set_alto_base(int newValue) { alto_base = newValue;}

		void set_pixel_ref_x(int newValue) { pixel_ref_x = newValue;}

		void set_pixel_ref_y(int newValue) { pixel_ref_y = newValue;}

		void set_fps(int newValue) { fps = newValue;}

		void set_delay(int newValue) { delay = newValue;}
		

};