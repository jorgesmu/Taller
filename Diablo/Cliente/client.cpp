#include "cliente.h"
#include <stdio.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27018"
#define DEFAULT_BUFLEN 512
	
using namespace std;

int main() {
	Cliente cliente;	
	cliente.obtenerDireccion("127.0.0.1",DEFAULT_PORT);
	cliente.crearSocket();	
	cliente.conectar();

	//modoEnvio();
	/*
	string texto;
	cin >> texto;
	while (true) {
		if (texto.compare("exit")==0) break;
		char *copy = new char[texto.size()+1] ;
		strcpy(copy, texto.c_str());
		cliente.enviar(copy,strlen(copy));
		cin >> texto;
	} */

	//modoRecepcion();
	int resultado=0;
	char buffer[DEFAULT_BUFLEN];
	while (true) {
		printf("El resultado es: %i",resultado);
		resultado = cliente.recibir(buffer,strlen(buffer));
		printf("El resultado es: %i",resultado);
		if (resultado > 0) {	
			 printf("%.*s\n", resultado, buffer);
		}
		if (resultado==0) break;
		if (resultado<0) printf("Error recepcion de datos: %d\n",WSAGetLastError());
	}

	cliente.desconectar();
	return 0;
}