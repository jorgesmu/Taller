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
	BOOL val_true = TRUE;
	int buff_size = 0;
	if(setsockopt(ListenSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&val_true, sizeof(BOOL)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS TCP_NODELAY\n";
	}
	//if(setsockopt(ListenSocket, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(int)) != 0) {
		//std::cerr << "ERROR SETTING SOCK OPTIONS SO_RCVBUF\n";
	//}
	if(setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(int)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS SO_SNDBUF\n";
	}
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
		BOOL val_true = TRUE;
		int buff_size = 0;
		if(setsockopt(tmp_sck, IPPROTO_TCP, TCP_NODELAY, (char*)&val_true, sizeof(BOOL)) != 0) {
			std::cerr << "ERROR SETTING SOCK OPTIONS TCP_NODELAY\n";
		}
		//if(setsockopt(tmp_sck, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(int)) != 0) {
			//std::cerr << "ERROR SETTING SOCK OPTIONS SO_RCVBUF\n";
		//}
		if(setsockopt(tmp_sck, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(int)) != 0) {
			std::cerr << "ERROR SETTING SOCK OPTIONS SO_SNDBUF\n";
		}
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
	EnterCriticalSection(&critSect);
	SOCKET sock = getClient(cid).sock;
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a send(): " << cid << "\n";
		LeaveCriticalSection(&critSect);
		return false;
	}
	LeaveCriticalSection(&critSect);
	return this->send(sock, msg);
}

// Funcion de send
bool ServerSocket::send(SOCKET sock, const std::string& msg) {
	EnterCriticalSection(&critSect);
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a send(): " << sock << "\n";
		LeaveCriticalSection(&critSect);
		return false;
	}

	BitStream bs;
	bs << msg;

	int res = ::send(sock, bs.str().c_str(), bs.str().size(), 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		LeaveCriticalSection(&critSect);
		return false;
	}else{
		//std::cout << "Se enviaron " << res << " bytes: " << bs.str() << "\n";
		LeaveCriticalSection(&critSect);
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
	std::string packet;
	int packet_size = -1;
	int bytes_read = 0;
	// Loop until we read a full packet
	while(true) {
		EnterCriticalSection(&critSect);
		std::string tmp;
		// If we have something in the queue, push it it
		if(queue_buf[cid].size() > 0) {
			tmp = queue_buf[cid];
			//std::cout << "APPENDING FROM QUEUE (" << queue_buf.size() << ")\n";
			bytes_read += queue_buf[cid].size();
			queue_buf.clear();
		}
		// If we have an empty packet, read the size first
		if(packet_size == -1 && !tmp.empty()) {
			// First 2 bytes 
			unsigned char buf[sizeof(short)];
			std::copy(tmp.begin(), tmp.begin()+sizeof(short), buf);
			packet_size = *(reinterpret_cast<short*>(&buf));
			packet_size += sizeof(short); // Add the size size
			//std::cout << "Got a packet with size: " << packet_size << "\n";
			//std::cout << tmp << "\n";
			tmp = tmp.substr(sizeof(short)); // Remove the 2 byte prefix		
		}

		// We finish the loop if we've read our packet size and we've already reached it
		if(packet_size != -1 ) {
			if(bytes_read > packet_size) {
				//std::cout << "Got more: " << bytes_read << "," << packet_size << "-----------------------------------\n";
				// Trim and store
				//std::cout << "Delta: " << bytes_read - packet_size << "\n";
				queue_buf[cid] = tmp.substr(tmp.size() - (bytes_read - packet_size));
				//std::cout << "STORING EXTRA (" << queue_buf.size() << ")\n";
				packet.append(tmp.substr(0, packet_size));
				//std::cout << "APPENDING " << packet_size << "\n";
				buff = packet;
				//std::cout << "DONE WITH PACKET\n";
				//std::cout << buff << "\n";
				LeaveCriticalSection(&critSect);
				return true;
			}else if(bytes_read == packet_size) {
				packet.append(tmp);
				buff = packet;
				//std::cout << "Got exact\n";
				//std::cout << "DONE WITH PACKET\n";
				//std::cout << buff << "\n";
				LeaveCriticalSection(&critSect);
				return true;
			}else if(bytes_read < packet_size) {
				//std::cout << "Got less||||||||||||||||||||||||||||||||||||||||\n";
				packet.append(tmp);
			}
		}
		LeaveCriticalSection(&critSect);
		int res = ::recv(sock, recvbuf, DEFAULT_BUFLEN, 0);
		EnterCriticalSection(&critSect);
		if(res > 0) {
			//bytes_read += res;
			//std::cout << "Read " << res << " bytes\n";
			//std::cout << "BytesRead: " << bytes_read << "\n";
			queue_buf[cid].append(recvbuf, res);
			//std::cout << "(" << queue_buf.size() << ") " << queue_buf << "\n";
		}else if(res == 0) {
			std::cout << "Connection closed\n";
			LeaveCriticalSection(&critSect);
			return false;
		}else{
			std::cout << "Receive error: " << WSAGetLastError() << "\n";
			LeaveCriticalSection(&critSect);
			return false;
		}
		LeaveCriticalSection(&critSect);
	}
	LeaveCriticalSection(&critSect);
	return false;
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
			std::cout << "Cierre de conexion para " << str_id << "\n";
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
	EnterCriticalSection(&critSect);
	// En teoria esto basta para cerrar todas las conexiones que tenia abiertas
	int res = closesocket(ListenSocket);
	if(res == SOCKET_ERROR) {
		std::cerr << "Cierre de conexion fallido: " << WSAGetLastError() << "\n";
	}else{
		std::cout << "Connection closed\n";
	}
	LeaveCriticalSection(&critSect);

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
		//LeaveCriticalSection(&critSect);
		std::stringstream ss;
		ss << "<" << cid << "> connected\n";
		std::cout << ss.str();
		// In this buffer we hold any incoming data
		std::string buff;

		// First we read a nick for the connected client
		if(!this->receive(cid, buff)) return;
		BitStream tmp_bs(buff.c_str(), buff.size());
		unsigned char bt;
		std::string new_nick, new_type;
		tmp_bs >> bt;
		tmp_bs >> new_nick;
		tmp_bs >> new_type;
		// Validamos
		if(bt != PROTO::NICKANDTYPE || new_nick.size() == 0 || new_type.size() == 0) {
			BitStream reply;
			reply << PROTO::TEXTMSG << std::string("Transaction error, valid nickname/type expected");
			this->send(cid, reply.str());
			removeClient(cid);
			LeaveCriticalSection(&critSect);
			return;
		}
		// Verificamos si el nick est� en uso
		for(auto it = clients_map.begin();it != clients_map.end();it++) {
			//std::cout << "NICK: " << it->second.nick << "\n";
			if(it->second.nick == new_nick) {
				BitStream reply;
				reply << PROTO::TEXTMSG << std::string("Nickname already connected");
				this->send(cid, reply.str());
				removeClient(cid);
				LeaveCriticalSection(&critSect);
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
			LeaveCriticalSection(&critSect);
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
		BitStream bs;
		bs << PROTO::INITPOS << pm.getPlayer(new_nick).getX() << pm.getPlayer(new_nick).getY();
		// al pedo: pm.getPlayer(new_nick).addTileRecorrido(pm.getPlayer(new_nick).getX(), pm.getPlayer(new_nick).getY());
		//std::cout << "SEND INIT POS (" << pm.getPlayer(new_nick).getX() << "," << pm.getPlayer(new_nick).getY() << ")\n";
		send(cid, bs.str());

		// Le mandamos el id escenario
		bs.clear();
		bs << PROTO::ESC_ID << escenario_elegido_id;
		//std::cout << "SEND ESC ID (" << escenario_elegido_id << ")\n";
		send(cid, bs.str());

		// Mandamos los tiles recorridos
		if(pm.getPlayer(new_nick).getTilesRecorridos().size() > 0) {
			bs.clear();
			bs << PROTO::NIEBLA_LIST << short(pm.getPlayer(new_nick).getTilesRecorridos().size());
			for(auto it = pm.getPlayer(new_nick).getTilesRecorridos().begin(); it != pm.getPlayer(new_nick).getTilesRecorridos().end();it++) {
				bs << it->first << it->second;
			}
			send(cid, bs.str());
		}

		// Para tipear menos, p_local = referencia al player de este thread
		const auto p_local = pm.getPlayer(new_nick);

		// Avisamos a los otros jugadores del nuevo jugador
		for(auto it = clients_map.begin();it != clients_map.end();it++) {
			if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
			BitStream bs;
			
			bs << PROTO::NEW_PLAYER << new_nick << p_local.getTipo() << p_local.getX() << p_local.getY() << p_local.isOn();
			send(it->second.sock, bs.str());
		}

		// Mandamos todos los otros players al que se unio
		for(auto it = clients_map.begin();it != clients_map.end();it++) {
			if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador			
			auto p = pm.getPlayer(it->second.nick);
			bs.clear();
			bs << PROTO::NEW_PLAYER << it->second.nick << p.getTipo() << p.getX() << p.getY() << p.isOn();
			send(cid, bs.str());
		}
		


		LeaveCriticalSection(&critSect);
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
				//std::cout << "RECEIVED NIEBLA SYNC: " << new_tile_x << "," << new_tile_y << "\n";
			}else{
				bs.clear();
				bs << PROTO::TEXTMSG << std::string("Unknown packet type");
				this->send(cid, bs.str());
			}
		}

		// Cuando hay una desconexion loggeamos e informamos al resto
		bs.clear();
		bs << PROTO::PLAYER_EXIT << new_nick;
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

	std::cout << "Sending " << fcount << " files to " << getClient(cid).nick << "\n";

	// Recorremos todos los archivos
	for(auto it = files.begin();it != files.end();it++) {
		std::string local_file = dir + "\\" + *it;
		// Mandamos el nombre del archivo
		bs.clear();
		bs << PROTO::FILE_HEADER << *it;
		if(!this->send(cid, bs.str())) return false;

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
		}
		bs.clear();
		bs << PROTO::FILE_DONE;
		if(!this->send(cid, bs.str())) return false;
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