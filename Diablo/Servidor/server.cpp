#include "servidor.h"
#include <stdio.h>
#include <iostream>
#include <string>

#define DEFAULT_PORT "27018"
#define DEFAULT_BUFLEN 10

using namespace std;

Servidor servidor;

void modoRecepcion() {
	int resultado;
	char buffer[DEFAULT_BUFLEN];
	while (true) {
		resultado = servidor.recibir(buffer,strlen(buffer));
		if (resultado > 0) {	
			 printf("%.*s\n", resultado, buffer);
		}
		if (resultado==0) break;
	}
}

int main() {
	Servidor servidor;
	servidor.generarDireccion(NULL,DEFAULT_PORT);
	servidor.crearSocket();
	servidor.escuchar();
	servidor.aceptarConexion();

	//modoRecepcion();
	/*
	int resultado;
	char buffer[DEFAULT_BUFLEN];
	while (true) {
		resultado = servidor.recibir(buffer,strlen(buffer));
		if (resultado > 0) {	
			 printf("%.*s\n", resultado, buffer);
		}
		if (resultado==0) break;
	}*/

	//modoEnvio();
	string texto;
	cin >> texto;
	while (true) {
		if (texto.compare("exit")==0) break;
		char *copy = new char[texto.size()+1] ;
		strcpy(copy, texto.c_str());
		servidor.enviar(copy,strlen(copy));
		cin >> texto;
	}

	servidor.cerrarSocket();
	return 0;
}