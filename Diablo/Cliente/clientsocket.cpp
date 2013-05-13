#include "../source/net/bitstream.h"
#include "../source/net/defines.h"
#include "../source/display/mapa.h"
#include "../../source/net/PjeManager.h"
#include "clientsocket.h"
#include <iostream>
#include <fstream>

using namespace std;
// Variables globales
extern bool pasoArchivos;
extern Mapa mapa;
extern PjeManager pm;
extern std::string pje_local_tipo;
extern int start_pos_x, start_pos_y;

bool ClientSocket::WSinit = false;
size_t ClientSocket::ref_count = 0;
ClientSocket::ClientSocket(CRITICAL_SECTION* main_cs) : main_cs(main_cs) {
	// Increase ref count
	ref_count++;
	// Init status
	open = false;
}

ClientSocket::~ClientSocket() {
	ref_count--;
	// Clean if this is the last socket
	if(ref_count == 0) {
		WSACleanup();
		WSinit = false;
	}
}

bool ClientSocket::init() {
	// Primero inicializamos winsock si no fue inicializado
	if(!WSinit) {
		WSADATA wsaData;
		int res = WSAStartup(MAKEWORD(2,2),&wsaData);
		if(res != 0) {
			std::cerr << "Inicializacion Winsock fallida: " << res << "\n";
			return false;
		}else{
			//std::cout << "Winsock inicializado\n";
			WSinit = true;
		}
	}
	// Creamos el socket
	ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket():" <<  WSAGetLastError() << "\n";
        return false;
    }else{
		//std::cout << "Socket creado\n";
		return true;
	}
}

// Funcion de conexion (solo para clientes)
bool ClientSocket::connect(const std::string& dir, int port) {
	// Cargamos los datos de conexion
	sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(dir.c_str());
    clientService.sin_port = htons(port);
	// Creamos la conexion
	if(::connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		std::cerr << "Error conectando a " << dir << ":" << port << ": " << WSAGetLastError() << "\n";
		this->close();
		return false;
	}else{
		std::cout << "Conectado a " << dir << ":" << port << "\n";
		open = true;
		return true;
	}

}

// Funcion de send
bool ClientSocket::send(const std::string& msg) {
	int res = ::send(ConnectSocket, msg.c_str(), msg.size(), 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		this->close();
		return false;
	}else{
		//std::cout << "Se enviaron " << res << " bytes\n";
		return true;
	}
}

bool ClientSocket::send(const char* msg, size_t size) {
	int res = ::send(ConnectSocket, msg, size, 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		this->close();
		return false;
	}else{
		//std::cout << "Se enviaron " << res << " bytes\n";
		return true;
	}
}

// Funcion de receive
bool ClientSocket::receive(std::string& buff) {
	int res = ::recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if(res > 0) {
		buff.clear();
		buff.assign(recvbuf, res);
		//std::cout << "Received " << res << " bytes: ";
		//std::cout.write(buff.c_str(), buff.size());
		//std::cout << "\n";
		return true;
	}else if(res == 0) {
		std::cout << "Connection closed\n";
		this->close();
		return false;
	}else{
		std::cout << "Receive error: " << WSAGetLastError() << "\n";
		this->close();
		return false;
	}
}

// Cierra el socket
void ClientSocket::close() {
	if(isOpen()) {
		int res = closesocket(ConnectSocket);
		if(res == SOCKET_ERROR) {
			std::cerr << "Cierre de conexion fallido: " << WSAGetLastError() << "\n";
		}else{
			std::cout << "Cierre de conexion exitosa\n";
		}
		open = false;
	}
}

unsigned int __stdcall ClientSocket::listenEntry(void* pthis) {
	ClientSocket* pt = (ClientSocket*)pthis;
	pt->listenDo();
	return 0;
}

void ClientSocket::listenDo() {
	std::string buff;
	while(this->receive(buff)) {
		EnterCriticalSection(this->main_cs);
		// This is for debugging purposes
		//std::cout << "(" << buff.size() << ")" << buff << "\n";

		// Build the bitstream
		BitStream bs(buff.c_str(), buff.size());
		// Branch based on packet type
		unsigned char pt;
		bs >> pt;

		if(pt == PROTO::TEXTMSG) {
			std::string msg;
			bs >> msg;
			std::cout << "Server says: " << msg << "\n";
		}else if(pt == PROTO::FILE_SEND_COUNT) {
			// Leemos la cantidad de archivos
			Uint16 fcount;
			bs >> fcount;
			std::cout << "Receiving " << fcount << " files\n";
			if(!sendOk()) return;

			// Loop de archivos
			for(int i = 0;i < fcount;i++) {
				// Esperamos el header ahora
				this->receive(buff);
				bs.load(buff.c_str(), buff.size());
				bs >> pt;
				if(pt != PROTO::FILE_HEADER) {
					std::cerr << "Unexpected packet, expected FILE_HEADER\n";
					this->close();
					return;
				}

				if(!sendOk()) return;

				std::string local_file;
				bs >> local_file;
				std::cout << "Receiving " << local_file << "...\n";
				std::ofstream f(std::string("../resources/")+local_file, std::ios_base::binary|std::ios_base::trunc);
				if(f.fail()) {
					std::cout << "Error opening " << local_file << "\n";
					this->close();
					return;
				}
				// Loopeamos hasta recibir todo el archivo
				while(this->receive(buff)) {
					//std::cout << buff << "\n";
					//std::cout << "--------------------------------------------\n";
					//std::cout << "--------------------------------------------\n";
					std::string tmp_chunk;
					bs.load(buff.c_str(), buff.size());
					bs >> pt;
					if(pt == PROTO::FILE_PART) {
						bs >> tmp_chunk;
						f.write(tmp_chunk.c_str(), tmp_chunk.size());
						//std::cout << "Writing chunk..\n";
						if(!sendOk()) return;
					}else if(pt == PROTO::FILE_DONE) {
						f.close();
						std::cout << "File done!\n";
						if(!sendOk()) return;
						break;
					}else{
						f.write(bs.data(), bs.size());
						//f.close();
						std::cerr << "Unexpected packet type during file transfer (" << int(pt) << "), aborting\n";
						this->close();
						return;
					}
				}
			}
			std::cout << "All files received\n";
			pasoArchivos = true;
		}else if(pt == PROTO::PREVIOUSTYPE) {
			std::string tipo;
			bs >> tipo;
			pje_local_tipo=tipo;
			sendOk();
		}else if(pt == PROTO::DEFTYPE) {
			std::string tipo;
			bs >> tipo;
			pje_local_tipo=tipo;
			sendOk();
		}else if(pt == PROTO::INITPOS) {
			bs >> start_pos_x;
			bs >> start_pos_y;
			sendOk();
		}else{
			std::cout << "Unknown packet type " << int(pt) << " received\n";
		}

		LeaveCriticalSection(this->main_cs);
	}
}

bool ClientSocket::isOpen() const {
	return this->open;
}

bool ClientSocket::sendOk() {
	return this->send("OK", 2);
}