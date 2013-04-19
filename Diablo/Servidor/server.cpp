#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512


int main() {
	WSADATA wsaData;
	int resultado;

	//Inicializa Winsock
	resultado=WSAStartup(MAKEWORD(2,2),&wsaData);
	if (resultado!=0) {
		printf("Inicializacion fallida: %d \n",resultado);
		return 1;
	}
	printf("Winsock inicializado \n");
	
	//Genera la address local
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Obtiene la address
	resultado = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (resultado != 0) {
		printf("Obtencion de address fallida: %d\n", resultado);
		WSACleanup();
		return 1;
	}
	printf("Address local generada \n");

	//Creo el socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		("Error al crear socket: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	printf("Socket creado \n");

	//Configura el socket para recibir conexiones
	resultado = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (resultado == SOCKET_ERROR) {
        printf("Conexion con cliente fallida: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
	freeaddrinfo(result); //Ya no es necesaria si la configuracion fue establecida con exito
	printf("Socket configurado \n");

	//Verifica si hay movimiento en la cola de conexiones
	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Deteccion de movimiento en cola fallida: %ld\n", WSAGetLastError() );
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Esperando una conexion \n");

	//Acepta una conexion
	SOCKET ClientSocket;

	ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("Aceptar conexion fallida: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Conexcion aceptada \n");

	//Envio/Recepcion de datos
	char recvbuf[DEFAULT_BUFLEN];
	int resulEnvio;
	int recvbuflen = DEFAULT_BUFLEN;

	// Recibir hasta que el cliente cierre la conexion
	do {
		resultado = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (resultado > 0) {
			printf("Bytes recibidos: %d\n", resultado);
			printf("El mensaje recibido es: %s\n",recvbuf);
			//Devuelve el buffer al cliente
			resulEnvio = send(ClientSocket, recvbuf, resultado, 0);
			if (resulEnvio == SOCKET_ERROR) {
				printf("Envio de buffer fallido: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			//printf("Bytes enviados: %d\n", resulEnvio);
		} else if (resultado == 0)
			printf("Cerrando conexion...\n");
		else {
			printf("Recepcion de datos fallida: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (resultado > 0);

	//Cierra el modo de envio
	resultado = shutdown(ClientSocket, SD_SEND);
	if (resultado == SOCKET_ERROR) {
		printf("Cierre modo envio fallido: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	//Cierra el socket
	closesocket(ClientSocket);
	WSACleanup();

	getchar();
	return 0;
}