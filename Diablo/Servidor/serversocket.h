#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include <vector>
#include <map>
#include <queue>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "../source/net/bitstream.h"
#include "../source/net/defines.h"

// Tamaño del buffer de envio/recepcion
const size_t DEFAULT_BUFLEN = 16384;
const size_t CHUNK_SIZE = 8192;
typedef std::string addr;
const std::string res_dir = "../Servidor/resources2client";

// Estructura POD con los datos que queremos guardar de cada conexion
struct Client {
	// Default ctor
	Client() : sock(NULL), nick("uninit-nick") { }
	// Members
	SOCKET sock; // Socket de conexion
	std::string nick; // Nick del usuario
};

class ServerSocket {
	private:
	// Init de winsock
	static bool WSinit;
	// Contador para hacer el manejor de recursos
	static size_t ref_count;
	// Variables para manejo del socket
	SOCKET ListenSocket;
	char recvbuf[DEFAULT_BUFLEN];
	std::map<std::string, std::string> queue_buf;
	// Mutex para el acceso a la data de clientes
	CRITICAL_SECTION critSect;
	// Queue de clientes aceptados pero que no estan siendo escuchados
	std::queue<std::string> clients_queue;
	// Tabla de clients
	std::map<std::string, Client> clients_map;
	// Funcion que agrega un cliente en la tabla
	void addClient(const sockaddr_in& tmp_st, const SOCKET& sock);
	// Construye un ID basado en la informacion de sockaddr_in
	static std::string buildId(const sockaddr_in& sckaddr);
	// Retorna un Client dado un ID o SOCKET_ERROR en caso de error
	Client& getClient(const std::string& id);
	// Devuelve un cid dado un nick o "" en caso de error
	std::string getCIDbyNick(const std::string& nick);
	public:
	// Constructor
	ServerSocket();
	// Destructor
	~ServerSocket();
	// Funcion de inicializacion
	bool init();
	// Abre el puerto especificado
	bool listen(int port);
	// Acepta una conexion entrante
	bool accept();
	// Funcion bloqueante de recepcion
	bool receive(const std::string& cid, std::string& buff);
	// Funcion de envio de archivos - id de conexion + lista de archivos
	bool sendFilesInDir(const std::string& cid, const std::string& dir);
	// Funciones para eliminar un cliente (desconectarlo)
	bool removeClient(const std::string& str_id);
	bool removeClient(const SOCKET& sock);
	// Funcion de send - toma como parametro el ID de la conexion a mandar y el mensaje
	bool send(const std::string& cid, const std::string& msg);
	bool send(SOCKET sock, const std::string& msg);
	// Funcion para enviar a todos los clientes
	bool sendAll(const std::string& msg);
	// Cierra el socket y todas sus conexiones
	void close();
	
	// Funcion de entry para el socket en modo receive
	static unsigned int __stdcall acceptLastEntry(void* pthis);
	// Funcion de listen 
	void acceptLastDo();

	// Funcion de entry para el socket en modo receive
	static unsigned int __stdcall listenLoopEntry(void* pthis);
	// Funcion de listen 
	void listenLoop();
	std::map<std::string, Client> ServerSocket::get_clients();
};