#include "cliente.h"
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27018"
#define DEFAULT_BUFLEN 512
	


int main() {
	int status=0;
	Cliente cliente;	
	status+=cliente.obtenerDireccion("127.0.0.1",DEFAULT_PORT);
	status+=cliente.crearSocket();	
	status+=cliente.conectar();
	if (status!=0) printf("Hubo un error en el proceso");
	char* texto="texto de prueba";
	cliente.enviar(texto,strlen(texto));
	char buffer[DEFAULT_BUFLEN];
	//Recibo hasta el fin de la conexion
	do {
		status=cliente.recibir(buffer,strlen(buffer));
		if (status > 0) {
			printf("Bytes recibidos: %d\n", status);
		}
		else if (status == 0) {
			printf("Conexion cerrada\n");
		}
		else {
			printf("Recepcion de datos erronea: %d\n", WSAGetLastError());
		}
	} while (status>0);
	
	getchar();
	return 0;
}