#include "config_servidor.h"
config_servidor::config_servidor(int puerto_destino){
	puerto = puerto_destino;
	completoPuerto = false;
}

//Getters
int config_servidor::get_puerto(){
	return puerto;
} 

bool config_servidor::get_completo_puerto(){
	return completoPuerto;	
} 

//Setters
void config_servidor::set_puerto(int newValue){
	puerto = newValue;
	completoPuerto = true;
} 