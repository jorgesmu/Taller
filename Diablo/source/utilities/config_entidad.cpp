#include "config_entidad.h"

//Constructor
config_entidad::config_entidad(std::string nombre_i, std::string path_imagen_i, int ancho_base_i, int alto_base_i, int pixel_ref_x_i, int pixel_ref_y_i, int fps_i, int delay_i) {
	nombre=nombre_i;
	path_imagen=path_imagen_i;
	ancho_base=ancho_base_i;
	alto_base=alto_base_i;
	pixel_ref_x=pixel_ref_x_i;
	pixel_ref_y=pixel_ref_y_i;
	fps=fps_i;
	delay=delay_i;
	completoNombre = false;
	completoPath = false;
	completoAncho = false;
	completoAlto = false;
	completoPixelX = false;
	completoPixelY = false;
	completoFps = false;
	completoDelay = false;
	completoAltoSprite = false;
	completoAnchoSprite = false;
	completoCaminable = false;
	esDefault = false;
}