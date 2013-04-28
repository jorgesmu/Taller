#include "serversocket.h"
#include <iostream>
#include <sstream>
#include <cassert>

#include <Windows.h>
#include <process.h>

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
		WSinit = false;
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
	// Creamos la critical section
	InitializeCriticalSection(&critSect);
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
bool ServerSocket::accept() {
	// Estructura para guardar info de la conexion entrante
	sockaddr_in tmp_st;
	int tmp_st_size = sizeof(tmp_st);
	// Socket para la nueva conexion
	SOCKET tmp_sck = ::accept(ListenSocket, (sockaddr*)&tmp_st, &tmp_st_size);
	if(tmp_sck == INVALID_SOCKET) {
		std::cerr << "Error during accept(): " << WSAGetLastError() << "\n";
		return false;
	}else{
		// Ingresamos la conexion en la tabla
		addClient(tmp_st, tmp_sck);
		// "Devolvemos" el id para la conexion que se acaba de aceptar
		clients_queue.push(buildId(tmp_st));
		// Spawneamos el thread
		_beginthreadex(NULL, 0, ServerSocket::acceptLastEntry, (void*)this, 0, NULL);
		return true;
	}
}

// Agrega un cliente a la tabla
void ServerSocket::addClient(const sockaddr_in& tmp_st, const SOCKET& sock) {
	//EnterCriticalSection(&critSect);
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
	//LeaveCriticalSection(&critSect);
}

SOCKET ServerSocket::getClient(const std::string& id) {
	SOCKET ret;
	//EnterCriticalSection(&critSect);
	if(clients_map.find(id) == clients_map.end()) {
		std::cerr << "Error en getClient(): " << id << " requested not found\n";
		ret = SOCKET_ERROR;
	}else{
		size_t pos = clients_map[id];
		std::cout << "Requested client <" << id << "> @ " << pos << "\n";
		// Checkeo
		assert(pos >= 0 && pos < clients_vector.size());
		ret = clients_vector[pos];
	}
	//LeaveCriticalSection(&critSect);
	return ret;
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
		removeClient(cid);
		return false;
	}else{
		std::cout << "Se enviaron " << res << " bytes\n";
		return true;
	}
}

bool ServerSocket::sendAll(const std::string& msg) {
	//EnterCriticalSection(&critSect);
	bool res = true;
	for(auto it = clients_map.begin(); it != clients_map.end(); it++) {
		res = res && this->send(it->first, msg);
	}
	//LeaveCriticalSection(&critSect);
	return res;
}

// Funcion de receive
bool ServerSocket::receive(const std::string& cid, std::string& buff) {

	SOCKET sock = getClient(cid);
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a receive(): " << cid << "\n";
		return false;
	}

	int res = ::recv(sock, recvbuf, DEFAULT_BUFLEN, 0);
	if(res > 0) {
		std::cout << "Received " << res << " bytes\n";
		buff.clear();
		buff.assign(recvbuf, res);
		return true;
	}else if(res == 0) {
		std::cout << "Connection closed\n";
		removeClient(cid);
		return false;
	}else{
		std::cout << "Receive error: " << WSAGetLastError() << "\n";
		removeClient(cid);
		return false;
	}
}

// Funciones para eliminar un cliente (desconectarlo)
bool ServerSocket::removeClient(const std::string& str_id) {
	bool ret;
	//EnterCriticalSection(&critSect);
	if(clients_map.find(str_id) == clients_map.end()) {
		std::cerr << "removeClient(): " << str_id << " not found\n";
		ret = false;
	}else{
		// Borramos
		size_t pos = clients_map[str_id];
		int res = closesocket(clients_vector[pos]);
		if(res == SOCKET_ERROR) {
			std::cerr << "Cierre de conexion en removeClient() fallido para " << str_id << ": " << WSAGetLastError() << "\n";
		}else{
			std::cout << "Cierre de conexion exitosa para " << str_id << "\n";
		}
		// Borramos de ambas estructuras 
		clients_vector.erase(clients_vector.begin()+pos);
		clients_map.erase(str_id);
		ret = true;
	}
	//LeaveCriticalSection(&critSect);
	return ret;
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

unsigned int __stdcall ServerSocket::acceptLastEntry(void* pthis) {
	ServerSocket* pt = (ServerSocket*)pthis;
	pt->acceptLastDo();
	return 0;
}

void ServerSocket::acceptLastDo() {
	if(clients_queue.empty()) {
		std::cerr << "Empty queue on acceptLastDo()\n";
		return;
	}else{
		std::string cid = clients_queue.front();
		clients_queue.pop();
		std::stringstream ss;
		ss << "<" << cid << "> connected\n";
		std::cout << ss.str();
		this->sendAll(ss.str());
		std::string buff;
		while(this->receive(cid, buff)) {
			std::stringstream ss;
			ss << "<" << cid << "> " << buff << "\n";
			std::cout << ss.str();
			this->sendAll(ss.str());
		}
		ss.str("");
		ss << "<" << cid << "> disconnected\n";
		std::cout << ss.str();
		this->sendAll(ss.str());
		return;
	}
}