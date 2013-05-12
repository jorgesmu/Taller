#include "config_cliente.h"
config_cliente::config_cliente(string ip_destino, int puerto_destino){
	ip_servidor = ip_destino;
	puerto = puerto_destino;
	completoIp = false;
	completoPuerto = false;
}

//Getters
string config_cliente::get_ip_servidor(){
	return ip_servidor;
} 

int config_cliente::get_puerto(){
	return puerto;
} 

bool config_cliente::get_completo_ip_servidor(){
	return completoIp;
}

bool config_cliente::get_completo_puerto(){
	return completoPuerto;	
} 

//Setters
void config_cliente::set_ip_server(string newValue){
	ip_servidor = newValue;
	completoIp = true;
}

void config_cliente::set_puerto(int newValue){
	puerto = newValue;
	completoPuerto = true;
} 