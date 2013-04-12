#include "config_escenario.h"

//Constructor
config_escenario::config_escenario(std::string nombre_inicial, int tam_x_inicial, int tam_y_inicial) {
	nombre=nombre_inicial;
	tam_x=tam_x_inicial;
	tam_y=tam_y_inicial;
	completoNombre = false;
	completoTamX = false;
	completoTamY = false;
	completoEntidades = false;
	completoProtagonista = false;

}