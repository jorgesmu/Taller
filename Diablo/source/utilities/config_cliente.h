#pragma once

#include <string>
using namespace std;
class config_cliente {

	private:
		string ip_servidor;
		int puerto;
		bool completoIp;
		bool completoPuerto;
	public:
		//Constructor
		config_cliente(string ip_destino, int puerto_destino);

		//Getters
		string get_ip_servidor(); 

		int get_puerto(); 

		bool get_completo_ip_servidor(); 

		bool get_completo_puerto(); 

		//Setters
		void set_ip_server(string newValue);

		void set_puerto(int newValue); 
};