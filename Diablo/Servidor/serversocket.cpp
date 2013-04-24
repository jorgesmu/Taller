#include "serversocket.h"
#include <iostream>
#include <sstream>
#include <cassert>

bool ServerSocket::WSinit = false;
size_t ServerSocket::ref_count = 0;

ServerSocket::ServerSocket() {
	// Increase ref count
	ref_count++;
}

ServerSocket::~ServerSocket() {
	ref_count--;
	// Clean if this is the last socket
	if(ref_count == 0) {
		WSACleanup();
	}
}

bool ServerSocket::init() {
	// Primero inicializamos winsock si no fue inicializado
	if(!WSinit) {
		WSADATA wsaData;
		int res = WSAStartup(MAKEWORD(2,2),&wsaData);
		if(res != 0) {
			std::cerr << "Inicializacion Winsock fallida: " << res << "\n";
			return false;
		}else{
			std::cout << "Winsock inicializado\n";
			WSinit = true;
		}
	}
	// Creamos el socket
	ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ListenSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket():" <<  WSAGetLastError() << "\n";
        return false;
    }else{
		std::cout << "Socket creado\n";
		return true;
	}
}

// Funcion de conexion (solo para clientes)
bool ServerSocket::listen(int port) {
	// Cargamos los datos de conexion
	sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverService.sin_port = htons(port);
	// Creamos la conexion
	if(::bind(ListenSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
		std::cerr << "Error haciendo bind() en puerto " << port << ": " << WSAGetLastError() << "\n";
		return false;
	}else{
		if (::listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			std::cerr << "Error en listen() en puerto " << port << ": " << WSAGetLastError() << "\n";
			return false;
		}else{
			std::cout << "Escuchando conexiones en puerto " << port << "\n";
			return true;
		}
	}

}

// Acepta una conexion entrante
bool ServerSocket::accept(std::string& str_id) {
	// Estructura para guardar info de la conexion entrante
	sockaddr_in tmp_st;
	int tmp_st_size = sizeof(tmp_st);
	// Socket para la nueva conexion
	SOCKET tmp_sck = ::accept(ListenSocket, (sockaddr*)&tmp_st, &tmp_st_size);
	if(tmp_sck == INVALID_SOCKET) {
		std::cerr << "Error during accept(): " << WSAGetLastError() << "\n";
		str_id = "";
		return false;
	}else{
		// Ingresamos la conexion en la tabla
		addClient(tmp_st, tmp_sck);
		// "Devolvemos" el id para la conexion que se acaba de aceptar
		str_id = buildId(tmp_st);
		return true;
	}
}

// Agrega un cliente a la tabla
void ServerSocket::addClient(const sockaddr_in& tmp_st, const SOCKET& sock) {
	// Ingresamos el socket en la lista
	clients_vector.push_back(sock);
	// Ingresamos el cliente por IP:Port en la tabla
	const std::string conn_id = buildId(tmp_st);
	// Un checkeo de que no exista ya esa conexion.. no deberia ocurrir nunca igual
	if(clients_map.find(conn_id) == clients_map.end()) {
		clients_map[conn_id] = clients_vector.size()-1;
		std::cout << "Accepted connection from " << conn_id  << "\n";
	}else{
		std::cerr << "Warning.. duplicate connection from " << conn_id << "\n";
	}
}

SOCKET ServerSocket::getClient(const std::string& id) {
	if(clients_map.find(id) == clients_map.end()) {
		std::cerr << "Error en getClient(): " << id << " requested not found\n";
		return SOCKET_ERROR;
	}else{
		size_t pos = clients_map[id];
		// Checkeo
		assert(pos >= 0 && pos < clients_vector.size());
		return clients_vector[pos];
	}
}

// Funcion de send
bool ServerSocket::send(const std::string& cid, const std::string& msg) {
	SOCKET sock = getClient(cid);
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a send(): " << cid << "\n";
		return false;
	}

	int res = ::send(sock, msg.c_str(), msg.size(), 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		return false;
	}else{
		std::cout << "Se enviaron " << res << " bytes\n";
		return true;
	}
}

// Funcion de receive
bool ServerSocket::receive(std::string& buff) {
	int res = ::recv(ListenSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if(res > 0) {
		std::cout << "Received " << res << " bytes\n";
		buff.clear();
		buff.assign(recvbuf, res);
		return true;
	}else if(res == 0) {
		std::cout << "Connection closed\n";
		return false;
	}else{
		std::cout << "Receive error: " << WSAGetLastError() << "\n";
		return false;
	}
}

// Funciones para eliminar un cliente (desconectarlo)
bool ServerSocket::removeClient(const std::string& str_id) {
	size_t pos;
	if(clients_map.find(str_id) == clients_map.end()) {
		std::cerr << "removeClient(): " << str_id << " not found\n";
		return false;
	}else{
		// Borramos
		pos = clients_map[str_id];
		int res = closesocket(clients_vector[pos]);
		if(res == SOCKET_ERROR) {
			std::cerr << "Cierre de conexion en removeClient() fallido para " << str_id << ": " << WSAGetLastError() << "\n";
		}else{
			std::cout << "Cierre de conexion exitosa para " << str_id << "\n";
		}
		// Borramos de ambas estructuras 
		clients_vector.erase(clients_vector.begin()+pos);
		clients_map.erase(str_id);
		return true;
	}
}

// Cierra el socket principal
void ServerSocket::close() {
	// En teoria esto basta para cerrar todas las conexiones que tenia abiertas
	int res = closesocket(ListenSocket);
	if(res == SOCKET_ERROR) {
		std::cerr << "Cierre de conexion fallido: " << WSAGetLastError() << "\n";
	}else{
		std::cout << "Cierre de conexion exitosa\n";
	}

}

// Construye un ID para la conexion 
std::string ServerSocket::buildId(const sockaddr_in& sckaddr) {
	std::stringstream conn_id;
	conn_id << inet_ntoa(sckaddr.sin_addr) << ":" << sckaddr.sin_port;
	return conn_id.str();
}