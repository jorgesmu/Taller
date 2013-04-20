#include "servidor.h"
#include <stdio.h>

#define DEFAULT_PORT "27018"
#define DEFAULT_BUFLEN 512

int main() {
	Servidor servidor;
	int resultado;
	servidor.generarDireccion(NULL,DEFAULT_PORT);
	servidor.crearSocket();
	servidor.escuchar();
	servidor.aceptarConexion();
	
	//Envio/Recepcion de datos
	char buffer[DEFAULT_BUFLEN];
	int resulEnvio;

	// Recibir hasta que el cliente cierre la conexion
	do {
		resultado = servidor.recibir(buffer,strlen(buffer));
		if (resultado > 0) {			
			//Devuelve el buffer al cliente
			resulEnvio=servidor.enviar(buffer,resultado);
			if (resulEnvio==1) servidor.cerrarSocket();
		} else if (resultado < 0) {
			servidor.cerrarSocket();
			return 1;
		}
	} while (resultado > 0);

	servidor.cerrarSocket();

	getchar();
	return 0;
}