#include "serversocket.h"

#include "../source/utilities/aux_func.h"
#include "parserServer.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <ctime>

#include <Windows.h>
#include <process.h>

#include "playerman.h"
#include "mapaservidor.h"

extern PlayerManager pm;
extern MapaServidor mapa;
extern std::vector <config_entidad> entidades;
extern int escenario_elegido_id;

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
	}
	// Inicializamos la critical section
	InitializeCriticalSection(&critSect);
	return true;
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
	EnterCriticalSection(&critSect);
	// Ingresamos el cliente por IP:Port en la tabla
	const std::string conn_id = buildId(tmp_st);
	// Un checkeo de que no exista ya esa conexion.. no deberia ocurrir nunca igual
	if(clients_map.find(conn_id) == clients_map.end()) {
		clients_map[conn_id].sock = sock;
		std::cout << "Accepted connection from " << conn_id  << "\n";
	}else{
		std::cerr << "Warning.. duplicate connection from " << conn_id << "\n";
	}
	LeaveCriticalSection(&critSect);
}

Client& ServerSocket::getClient(const std::string& id) {
	EnterCriticalSection(&critSect);
	if(clients_map.find(id) == clients_map.end()) {
		std::cerr << "Error en getClient(): " << id << " requested not found\n";
		// Fake error-reporting client
		Client& ret = clients_map["0"];
		ret.sock = SOCKET_ERROR;
		ret.nick = "INVALID-CLIENT";
		LeaveCriticalSection(&critSect);
		return ret;
	}else{
		LeaveCriticalSection(&critSect);
		return clients_map[id];
		//std::cout << "Requested client <" << id << ">\n";
	}
}

// Funcion de send
bool ServerSocket::send(const std::string& cid, const std::string& msg) {
	SOCKET sock = getClient(cid).sock;
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a send(): " << cid << "\n";
		return false;
	}

	int res = ::send(sock, msg.c_str(), msg.size(), 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		return false;
	}else{
		//std::cout << "Se enviaron " << res << " bytes\n";
		return true;
	}
}

bool ServerSocket::send(const std::string& cid, const char* msg, size_t size) {
	SOCKET sock = getClient(cid).sock;
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a send(): " << cid << "\n";
		return false;
	}

	int res = ::send(sock, msg, size, 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		return false;
	}else{
		//std::cout << "Se enviaron " << res << " bytes\n";
		return true;
	}
}

bool ServerSocket::sendAll(const std::string& msg) {
	EnterCriticalSection(&critSect);
	bool res = true;
	for(auto it = clients_map.begin(); it != clients_map.end(); it++) {
		res = res && this->send(it->first, msg);
	}
	LeaveCriticalSection(&critSect);
	return res;
}

// Funcion de receive
bool ServerSocket::receive(const std::string& cid, std::string& buff) {

	SOCKET sock = getClient(cid).sock;
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a receive(): " << cid << "\n";
		return false;
	}

	int res = ::recv(sock, recvbuf, DEFAULT_BUFLEN, 0);
	if(res > 0) {
		//std::cout << "Received " << res << " bytes: ";
		//std::cout.write(buff.c_str(), buff.size());
		//std::cout << "\n";
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
	bool ret;
	EnterCriticalSection(&critSect);
	if(clients_map.find(str_id) == clients_map.end()) {
		std::cerr << "removeClient(): " << str_id << " not found\n";
		ret = false;
	}else{
		// Borramos
		int res = closesocket(clients_map[str_id].sock);
		if(res == SOCKET_ERROR) {
			std::cerr << "Cierre de conexion en removeClient() fallido para " << str_id << ": " << WSAGetLastError() << "\n";
		}else{
			std::cout << "Cierre de conexion exitosa para " << str_id << "\n";
		}
		// Borramos
		clients_map.erase(str_id);
		ret = true;
	}
	LeaveCriticalSection(&critSect);
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

unsigned int __stdcall ServerSocket::listenLoopEntry(void* pthis) {
	ServerSocket* pt = (ServerSocket*)pthis;
	pt->listenLoop();
	return 0;
}

void ServerSocket::listenLoop() {
	// Aceptar conexiones indefinidamente
	while(accept()) {
		//
	}
}

unsigned int __stdcall ServerSocket::acceptLastEntry(void* pthis) {
	ServerSocket* pt = (ServerSocket*)pthis;
	pt->acceptLastDo();
	return 0;
}

void ServerSocket::acceptLastDo() {

	// Iniciamos el seed de srand
	std::srand(std::time(NULL));

	EnterCriticalSection(&critSect);
	if(clients_queue.empty()) {
		std::cerr << "Empty queue on acceptLastDo()\n";
		LeaveCriticalSection(&critSect);
		return;
	}else{
		std::string cid = clients_queue.front();
		clients_queue.pop();
		LeaveCriticalSection(&critSect);
		std::stringstream ss;
		ss << "<" << cid << "> connected\n";
		std::cout << ss.str();
		// In this buffer we hold any incoming data
		std::string buff;

		// First we read a nick for the connected client
		this->receive(cid, buff);
		BitStream tmp_bs(buff.c_str(), buff.size());
		unsigned char bt;
		std::string new_nick, new_type;
		tmp_bs >> bt;
		tmp_bs >> new_nick;
		tmp_bs >> new_type;
		// Validamos
		if(bt != PROTO::NICKANDTYPE || new_nick.size() == 0 || new_type.size() == 0) {
			BitStream reply;
			reply << PROTO::TEXTMSG << "Transaction error, valid nickname/type expected";
			this->send(cid, reply.str());
			removeClient(cid);
			return;
		}
		// Verificamos si el nick est� en uso
		for(auto it = clients_map.begin();it != clients_map.end();it++) {
			//std::cout << "NICK: " << it->second.nick << "\n";
			if(it->second.nick == new_nick) {
				BitStream reply;
				reply << PROTO::TEXTMSG << "Nickname already connected";
				this->send(cid, reply.str());
				removeClient(cid);
				return;
			}
		}
		// Asignamos si todo marcho como esperabamos
		getClient(cid).nick = new_nick;
		std::cout << new_nick << " - connected from " << cid << "\n";

		// Hacemos el envio inicial de todos los archivos
		if(!sendFilesInDir(cid, res_dir)) {
			std::cout << "Error enviando archivos a " << cid << " - " << getClient(cid).nick << "\n";
			removeClient(cid);
			return;
		}

		// Agregamos al playermanager si no existe
		if(pm.playerExists(new_nick)) {
			std::string tipo = pm.getPlayer(new_nick).getTipo();
			BitStream bs;
			bs << PROTO::PREVIOUSTYPE << tipo;
			send(cid, bs.str());
		}else{
			bool found=false;
			for (auto it=entidades.begin();it!=entidades.end();it++) {
				if (it->get_nombre()==new_type) {
					found=true;
					break;
				}
			}
			if(!found) {
				BitStream bs;
				std::string tipo="nohay";
				for (auto it=entidades.begin();it!=entidades.end();it++) {
					if (it->get_es_default()) {
						tipo=it->get_nombre();
						break;
					}
				}
				bs << PROTO::DEFTYPE << tipo;
				send(cid, bs.str());
				new_type = tipo;
			}
			// Agregamos el jugador
			pm.addPlayer(new_nick, new_type, mapa);

		}
		
		// Le mandamos la posicion inicial
		waitForOk(cid);
		BitStream bs;
		bs << PROTO::INITPOS << pm.getPlayer(new_nick).getX() << pm.getPlayer(new_nick).getY();
		// al pedo: pm.getPlayer(new_nick).addTileRecorrido(pm.getPlayer(new_nick).getX(), pm.getPlayer(new_nick).getY());
		//std::cout << "SEND INIT POS (" << pm.getPlayer(new_nick).getX() << "," << pm.getPlayer(new_nick).getY() << ")\n";
		send(cid, bs.str());
		waitForOk(cid);

		// Le mandamos el id escenario
		bs.clear();
		bs << PROTO::ESC_ID << escenario_elegido_id;
		//std::cout << "SEND ESC ID (" << escenario_elegido_id << ")\n";
		send(cid, bs.str());
		waitForOk(cid);

		// Mandamos los tiles recorridos
		if(pm.getPlayer(new_nick).getTilesRecorridos().size() > 0) {
			bs.clear();
			bs << PROTO::NIEBLA_LIST << short(pm.getPlayer(new_nick).getTilesRecorridos().size());
			for(auto it = pm.getPlayer(new_nick).getTilesRecorridos().begin(); it != pm.getPlayer(new_nick).getTilesRecorridos().end();it++) {
				bs << it->first << it->second;
			}
			send(cid, bs.str());
		}

		// Mandamos todos los otros players
		//


		// Receive loop
		while(this->receive(cid, buff)) {

			// This is for debugging purposes
			std::stringstream ss;
			//ss << "<" << cid << "> " << buff << "\n";
			//std::cout << ss.str();

			// Build the bitstream
			BitStream bs(buff.c_str(), buff.size());
			// Branch based on packet type
			unsigned char pt;
			bs >> pt;

			if(pt == PROTO::CHAT) {
				bs.clear();
				ss.str("");
				ss << "<" << getClient(cid).nick << "> " << buff.substr(1, buff.size());
				bs << PROTO::TEXTMSG << ss.str();
				this->sendAll(bs.str());
			}else if(pt == PROTO::NIEBLA_SYNC) {
				int new_tile_x, new_tile_y;
				bs >> new_tile_x >> new_tile_y;
				pm.getPlayer(new_nick).addTileRecorrido(new_tile_x, new_tile_y);
				std::cout << "RECEIVED NIEBLA SYNC: " << new_tile_x << "," << new_tile_y << "\n";
			}else{
				bs.clear();
				bs << PROTO::TEXTMSG << "Unknown packet type";
				this->send(cid, bs.str());
			}
		}

		// Cuando hay una desconexion loggeamos e informamos al resto
		ss.str("");
		ss << getClient(cid).nick << " - disconnected\n";
		bs.clear();
		bs << PROTO::TEXTMSG << ss.str();
		removeClient(cid);
		this->sendAll(bs.str());
		

		return;
	}
}

bool ServerSocket::sendFilesInDir(const std::string& cid, const std::string& dir) {


	std::string buff;
	// Cargamos los archivos
	std::vector<std::string> files;
	listFilesinDir(dir, files);

	// Primero mandamos la cantidad de archivos
	BitStream bs;
	Uint16 fcount = files.size();
	bs << PROTO::FILE_SEND_COUNT << fcount;
	if(!this->send(cid, bs.str()))
		return false;

	if(!waitForOk(cid)) return false;

	std::cout << "Sending " << fcount << " files to " << getClient(cid).nick << "\n";

	// Recorremos todos los archivos
	for(auto it = files.begin();it != files.end();it++) {
		std::string local_file = dir + "\\" + *it;
		// Mandamos el nombre del archivo
		bs.clear();
		bs << PROTO::FILE_HEADER << *it;
		if(!this->send(cid, bs.str())) return false;

		if(!waitForOk(cid)) return false;

		// Mandamos los chunks
		size_t fs = fileSize(local_file);
		std::ifstream f(local_file, std::ios_base::binary);

		char tmp_buf[CHUNK_SIZE];
		size_t csize;
		while(fs > 0) {
			bs.clear();
			bs << PROTO::FILE_PART;
			if(fs > CHUNK_SIZE) {
				csize = CHUNK_SIZE;
				f.read(tmp_buf, CHUNK_SIZE);
			}else{
				csize = fs;
				f.read(tmp_buf, fs);
			}
			std::string tmp_str(tmp_buf, csize);
			bs << tmp_str;
			fs -= csize;
			if(!this->send(cid, bs.str())) return false;
			//std::cout << "Chunk sent, size: " << csize << "\n";
			if(!waitForOk(cid)) return false;
		}
		bs.clear();
		bs << PROTO::FILE_DONE;
		if(!this->send(cid, bs.str())) return false;
		if(!waitForOk(cid)) return false;
		f.close();
	}

	std::cout << "Envio completado\n";

	return true;
}

std::string ServerSocket::getCIDbyNick(const std::string& nick) {
	EnterCriticalSection(&critSect);
	for(auto it = clients_map.begin();it != clients_map.end();it++) {
		if(it->second.nick == nick) {
			LeaveCriticalSection(&critSect);
			return it->first;
		}
	}
	LeaveCriticalSection(&critSect);
	return "";
}

bool ServerSocket::waitForOk(const std::string& cid) {
	std::string buff;
	//std::cout << "Waiting for ok...";
	// Esperamos la respuesta
	if(!this->receive(cid, buff)) return false;
	if(buff != "OK") {
		std::cout << "Malformed OK received\n";
		return false;
	}
	//std::cout << "ACK\n";
	return true;
}