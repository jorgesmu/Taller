#include "servidor.h"

Servidor::Servidor() {
	this->result=NULL;
	this->ptr=NULL;
}

//Inicializa el Winsock
int inicializar() {
	WSADATA wsaData;
	//Inicializa Winsock
	int resultado=WSAStartup(MAKEWORD(2,2),&wsaData);
	if (resultado!=0) {
		printf("Inicializacion fallida: %d \n",resultado);
		return 1;
	}
	printf("Winsock inicializado \n");
	return 0;
}

//Genera una direccion local con los datos
int Servidor::generarDireccion(char* dir,char* port) {
	inicializar();	
	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	int resultado = getaddrinfo(dir, port, &hints, &result);
	if (resultado != 0) {
		printf("Error al generar direccion local: %d\n", resultado);
		WSACleanup();
		return 1;
	}
	printf("Direccion local generada \n");
	return 0;
}

//Configura el socket para recibir conexiones
int Servidor::configurarSocket() {	
	int resultado = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (resultado == SOCKET_ERROR) {
        printf("Configuracion de socket fallida: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
	freeaddrinfo(result); //Ya no es necesaria si la configuracion fue establecida con exito
	printf("Socket configurado \n");
	return 0;
}

//Crea un nuevo socket
int Servidor::crearSocket() {
	ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		("Error al crear socket: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	printf("Socket creado \n");
	configurarSocket();
	return 0;
}

//Verifica si hay movimiento en la cola de conexiones
int Servidor::escuchar() {	
	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Deteccion de movimiento en cola fallida: %ld\n", WSAGetLastError() );
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Esperando una conexion \n");
	return 0;
}

//Acepta un pedido de conexion entrante
int Servidor::aceptarConexion() {
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("Aceptar conexion fallida: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Conexcion aceptada \n");
	return 0;
}

//Recibe datos del cliente
int Servidor::recibir(char* buffer,int len)  {
	int resultado = recv(ClientSocket, buffer, len, 0);
	if (resultado<0) {
		printf("Error al recibir datos del cliente\n");
	} else if (resultado>0) {
		//printf("Bytes recibidos: %d\n", resultado);
		//printf("El mensaje recibido es: %s\n",buffer);
	} else {
		//printf("Conexion cerrada\n");
	}
	return resultado;
}

//Envia datos al cliente
int Servidor::enviar(char* data,int len) {
	int resultado=0;
	resultado=send(ClientSocket, data, resultado, 0);
	if (resultado == SOCKET_ERROR) {
		printf("Envio de buffer fallido: %d\n", WSAGetLastError());
		return 1;
	}
	printf("Envio de datos correcto\n");
	return 0;
}

//Cierra la conexion con el cliente
int Servidor::cerrarConexion() {
	int resultado = shutdown(ClientSocket, SD_SEND);
	if (resultado == SOCKET_ERROR) {
		printf("Cierre de conexion fallido: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	printf("Cierre de conexion exitosa\n");
	return 0;
}

//Cierra el socket
int Servidor::cerrarSocket() {
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}