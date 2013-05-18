#include "config_entidad_en_juego.h"
#include <string>

//Constructor
config_entidad_en_juego::config_entidad_en_juego(string nombreValue, int pos_x_value, int pos_y_value) {
	nombre = nombreValue;
	pos_x = pos_x_value;
	pos_y = pos_y_value;
	completoNombre = false;
	completoPosX = false;
	completoNombre = false;
	completoPosY = false;
};