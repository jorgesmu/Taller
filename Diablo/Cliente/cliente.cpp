#include "cliente.h"

Cliente::Cliente() {
	this->result=NULL;
	this->ptr=NULL;
}

//Inicializa Winsock
int inicializar() {
	WSADATA wsaData;
	int res=WSAStartup(MAKEWORD(2,2),&wsaData);
	if (res!=0) {
		printf("Inicializacion Winsock fallida: %d \n",res);
		return 1;
	}
	printf("Winsock inicializado \n");
	return 0;
}

//Obtiene la direccion del host a partir de los datos
int Cliente::obtenerDireccion(char *dir,char* port) {
	inicializar();
	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;

	int resultado=getaddrinfo(dir,port,&hints,&result);
	if (resultado!=0) {
		printf("Obtencion de direccion fallida: %d\n",resultado);
		WSACleanup();
		return 1;
	}
	printf("Direccion del host obtenida \n");
	return 0;
}

//Crea un nuevo socket para establecer la conexion
int Cliente::crearSocket() {
	ConnectSocket=INVALID_SOCKET;
	ptr=result;
	ConnectSocket=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
	if (ConnectSocket==INVALID_SOCKET) {
		printf("Error al crear socket: %ld\n",WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	printf("Socket creado \n");
	return 0;
}

//Conecta el socket con el host
int Cliente::conectar() {
	int resultado=connect(ConnectSocket,ptr->ai_addr,(int)ptr->ai_addrlen);
	if (resultado==SOCKET_ERROR) {
		printf("Error al intentar conectar al servidor\n");
		closesocket(ConnectSocket);
		ConnectSocket=INVALID_SOCKET;
		return 1;
	}
	freeaddrinfo(result);
	if (ConnectSocket==INVALID_SOCKET) {
		printf("Error de conexion-Socket invalido\n");
		WSACleanup();
		return 1;
	}
	printf("Conexion establecida \n");
	return 0;
}

//Envia datos al host
int Cliente::enviar(char* data,int len) {
	int resultado=send(ConnectSocket,data,len,0);
	if (resultado==SOCKET_ERROR) {
		printf("Error al enviar data: %d\n",WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	//printf("Bytes enviados: %ld\n",resultado);
	//printf("Mensaje enviado: %s \n",data);
	/*
	//Dejo de enviar
	resultado=shutdown(ConnectSocket,SD_SEND);
	if (resultado==SOCKET_ERROR) {
		printf("Termino de envio fallido: %d\n",WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Proceso de envio finalizado correctamente \n");
	*/
	return 0;
}

//Recibe datos del host
int Cliente::recibir(char* buffer,int len) {
	int resultado=recv(ConnectSocket,buffer,len,0);
	return resultado;
}

//Termina la conexion con el host
int Cliente::desconectar() {
	int resultado=shutdown(ConnectSocket,SD_SEND);
	if (resultado==SOCKET_ERROR) {
		printf("Termino de envio fallido: %d\n",WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();
	printf("Conexion finalizada \n");
	return 0;
}