#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class Cliente {
	private:
		//Estructura para la address
		struct addrinfo *result,
						*ptr,
						hints;

		SOCKET ConnectSocket;


	public:
		//Nueva instancia de cliente
		Cliente();

		//Obtiene la direccion del host a partir de los datos
		int obtenerDireccion(char *dir,char* port);

		//Crea un nuevo socket para establecer la conexion
		int crearSocket();

		//Conecta el socket con el host
		int conectar();

		//Envia datos al host
		int enviar(char* data,int len);

		//Recibe datos del host
		int recibir(char* buffer,int len);

		//Termina la conexion con el host
		int desconectar();

};