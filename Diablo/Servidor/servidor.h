#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class Servidor {
	private:
		struct addrinfo *result, 
						*ptr, 
						hints;

		SOCKET ListenSocket;
		SOCKET ClientSocket;

	public:
		//Nueva instancia de Servidor
		Servidor();

		//Genera una direccion local con los datos
		int generarDireccion(char* dir,char* port);

		//Configura el socket para recibir conexiones
		int configurarSocket();

		//Crea un nuevo socket
		int crearSocket();

		//Verifica si hay movimiento en la cola de conexiones
		int escuchar();

		//Acepta un pedido de conexion entrante
		int aceptarConexion();

		//Recibe datos del cliente
		int recibir(char* buffer,int len);

		//Envia datos al cliente
		int enviar(char* data,int len);

		//Cierra la conexion con el cliente
		int cerrarConexion();

		//Cierra el socket
		int cerrarSocket();
};