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

	//Estructura para la address
	struct addrinfo *result=NULL,
					*ptr=NULL,
					hints;

	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;

	//Obtiene address a partir del nombre
	resultado=getaddrinfo("127.0.0.1",DEFAULT_PORT,&hints,&result);
	if (resultado!=0) {
		printf("Obtencion de direccion a partir de nombre fallida: %d\n",resultado);
		WSACleanup();
		return 1;
	}
	printf("Address del host obtenida \n");

	//Creo Socket
	SOCKET ConnectSocket=INVALID_SOCKET;
	ptr=result;
	ConnectSocket=socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);
	if (ConnectSocket==INVALID_SOCKET) {
		printf("Error al crear el socket: %ld\n",WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	printf("Socket creado \n");

	//Conecto el socket
	resultado=connect(ConnectSocket,ptr->ai_addr,(int)ptr->ai_addrlen);
	if (resultado==SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket=INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (ConnectSocket==INVALID_SOCKET) {
		printf("No fue posible conectarse al servidor.\n");
		WSACleanup();
		return 1;
	}
	printf("Conexion establecida \n");

	//Envio de datos al servidor
	int recvbuflen=DEFAULT_BUFLEN;

	char* sendbuf="Texto de Prueba";
	char recvbuf[DEFAULT_BUFLEN];

	resultado=send(ConnectSocket,sendbuf,(int)strlen(sendbuf),0);
	if (resultado==SOCKET_ERROR) {
		printf("Error al intentar enviar mensaje: %d\n",WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes enviados: %ld\n",resultado);
	printf("Mensaje enviado: %s \n",sendbuf);

	//Dejo de enviar
	resultado=shutdown(ConnectSocket,SD_SEND);
	if (resultado==SOCKET_ERROR) {
		printf("Termino de envio fallido: %d\n",WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Inicio modo recepcion de datos \n");

	//Recibo hasta el fin de la conexion
	do {
		resultado=recv(ConnectSocket,recvbuf,recvbuflen,0);
		if (resultado > 0) {
			printf("Bytes recibidos: %d\n", resultado);
		}
		else if (resultado == 0) {
			printf("Conexion cerrada\n");
		}
		else {
			printf("Recepcion de datos erronea: %d\n", WSAGetLastError());
		}
	} while (resultado>0);

	//Desconecta el socket
	closesocket(ConnectSocket);
	WSACleanup();
	printf("Conexion finalizada \n");
	
	getchar();
	return 0;
}