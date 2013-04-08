#include "config_general.h"

//Constructores
config_general::config_general(int vel_personaje_i, int margen_scroll_i) {
	vel_personaje=vel_personaje_i;
	margen_scroll=margen_scroll_i;
}
config_general::config_general() {
	//-1 es un codigo de error definido por defecto (luego modificar por constantes)
	vel_personaje=-1;
	margen_scroll=-1;
}
