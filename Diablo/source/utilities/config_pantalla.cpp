#include "config_pantalla.h"

//Constructores
config_pantalla::config_pantalla(int ancho_i, int alto_i) {
	ancho=ancho_i;
	alto=alto_i;
	completo = false;
	completoAlto = false;
	completoAncho = false;
}
config_pantalla::config_pantalla() {
	//-1 es un valor de error que definimos por defecto
	ancho= -1;
	alto= -1;
}