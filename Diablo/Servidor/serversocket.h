#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <vector>
#include <map>

#include <winsock2.h>
#include <ws2tcpip.h>

// Tamaño del buffer de envio/recepcion
const size_t DEFAULT_BUFLEN = 1024;
typedef std::string addr;

class ServerSocket {
	private:
	// Init de winsock
	static bool WSinit;
	// Contador para hacer el manejor de recursos
	static size_t ref_count;
	// Variables para manejo del socket
	SOCKET ListenSocket;
	char recvbuf[DEFAULT_BUFLEN];
	// Vector de sockets conectados
	std::vector<SOCKET> clients_vector;
	// Tabla de clients
	std::map<std::string, size_t> clients_map;
	// Funcion que agrega un cliente en la tabla
	void addClient(const sockaddr_in& tmp_st, const SOCKET& sock);
	// Construye un ID basado en la informacion de sockaddr_in
	static std::string buildId(const sockaddr_in& sckaddr);
	// Retorna un SOCKET dado un ID o SOCKET_ERROR en caso de error
	SOCKET getClient(const std::string& id);
	public:
	// Constructor
	ServerSocket();
	// Destructor
	~ServerSocket();
	// Funcion de inicializacion
	bool init();
	// Abre el puerto especificado
	bool listen(int port);
	// Acepta una conexion entrante y guarda el id en el string pasado
	bool accept(std::string& str_id);
	// Funcion bloqueante de recepcion
	bool receive(std::string& buff);
	// Funciones para eliminar un cliente (desconectarlo)
	bool removeClient(const std::string& str_id);
	// Funcion de send - toma como parametro el ID de la conexion a mandar y el mensaje
	bool send(const std::string& cid, const std::string& msg);
	// Cierra el socket y todas sus conexiones
	void close();
};