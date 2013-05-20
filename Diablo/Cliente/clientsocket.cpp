#include "../source/net/bitstream.h"
#include "../source/net/defines.h"
#include "../source/display/mapa.h"
#include "../../source/net/PjeManager.h"
#include "../../source/utilities/config_cliente.h"
#include "../../source/utilities/parser.h"
#include "clientsocket.h"
#include <iostream>
#include <fstream>

using namespace std;
// Variables globales
extern bool pasoArchivos;
extern bool cargoMapa;
extern Mapa mapa;
extern PjeManager pjm;
extern std::string pje_local_tipo;
extern int start_pos_x, start_pos_y;
extern int escenario_elegido_id;
extern config_general configuracion;
extern ResMan resman;

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
	BOOL val_true = TRUE;
	int buff_size = 0;
	if(setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&val_true, sizeof(BOOL)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS TCP_NODELAY\n";
	}
	//if(setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(int)) != 0) {
		//std::cerr << "ERROR SETTING SOCK OPTIONS SO_RCVBUF\n";
	//}
	if(setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(int)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS SO_SNDBUF\n";
	}
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
	BitStream bs;
	bs << msg;
	int res = ::send(ConnectSocket, bs.str().c_str(), bs.str().size(), 0);
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
	std::string packet;
	int packet_size = -1;
	int bytes_read = 0;
	// Loop until we read a full packet
	while(true) {
		std::string tmp;
		// If we have something in the queue, push it it
		if(queue_buf.size() > 0) {
			tmp = queue_buf;
			//std::cout << "APPENDING FROM QUEUE (" << queue_buf.size() << ")\n";
			bytes_read += queue_buf.size();
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
				queue_buf = tmp.substr(tmp.size() - (bytes_read - packet_size));
				//std::cout << "STORING EXTRA (" << queue_buf.size() << ")\n";
				packet.append(tmp.substr(0, packet_size));
				//std::cout << "APPENDING " << packet_size << "\n";
				buff = packet;
				//std::cout << "DONE WITH PACKET\n";
				//std::cout << buff << "\n";
				return true;
			}else if(bytes_read == packet_size) {
				packet.append(tmp);
				buff = packet;
				//std::cout << "Got exact\n";
				//std::cout << "DONE WITH PACKET\n";
				//std::cout << buff << "\n";
				return true;
			}else if(bytes_read < packet_size) {
				//std::cout << "Got less||||||||||||||||||||||||||||||||||||||||\n";
				packet.append(tmp);
			}
		}

		int res = ::recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if(res > 0) {
			//bytes_read += res;
			//std::cout << "Read " << res << " bytes\n";
			//std::cout << "BytesRead: " << bytes_read << "\n";
			queue_buf.append(recvbuf, res);
			//std::cout << "(" << queue_buf.size() << ") " << queue_buf << "\n";
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
					}else if(pt == PROTO::FILE_DONE) {
						f.close();
						std::cout << "File done!\n";
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
		}else if(pt == PROTO::DEFTYPE) {
			std::string tipo;
			bs >> tipo;
			pje_local_tipo=tipo;
		}else if(pt == PROTO::INITPOS) {
			bs >> start_pos_x;
			bs >> start_pos_y;
			//std::cout << "RECEIVED INIT POS: (" << start_pos_x << "," << start_pos_y << ")\n";
		}else if(pt == PROTO::ESC_ID) {
			bs >> escenario_elegido_id;
			//std::cout << "RECEIVED ESC ID: (" << escenario_elegido_id << ")\n";
		}else if(pt == PROTO::NIEBLA_LIST) {
			// Esperamos a que cargue el mapa
			while(!cargoMapa) {
				Sleep(10);
			}
			short tile_list_size;
			bs >> tile_list_size;
			//std::cout << "RECEIVED NIEBLA_LIST (" << tile_list_size << "): \n";
			for(int i = 0;i < tile_list_size;i++) {
				short x, y;
				bs >> x >> y;
				Tile::setearExplorados(x, y, &pjm.getPjeLocal(), &mapa);
				//std::cout << x << "," << y << " ";
			}
			//std::cout << "\n";
		}else if(pt == PROTO::NEW_PLAYER) {
			// Esperamos a que cargue el mapa
			while(!cargoMapa) {
				Sleep(10);
			}
			std::string new_nick, new_type;
			int x, y;
			bool is_on;
			bs >> new_nick >> new_type >> x >> y >> is_on;
			//std::cout << "RECEIVED NEW_PLAYER: " << new_nick << " " << new_type << x << "," << y << " " << is_on << "\n";
			pjm.addPje(new_nick);
			auto& p = pjm.getPje(new_nick);
			// Agrega el personaje
			p.init(new_nick, pje_local_tipo , 1 , 1 , 50 , 5, 100, 100 ,	configuracion.get_vel_personaje(),	0 , 70 , NULL , resman , Imagen::COLOR_KEY);
			// Posiciono el personaje
			mapa.getTile(x, y)->addEntidad(&p);
			p.setTileActual(mapa.getTile(x, y));
		}else{
			std::cout << "Unknown packet type " << int(pt) << " received\n";
		}

		LeaveCriticalSection(this->main_cs);
	}
}

bool ClientSocket::isOpen() const {
	return this->open;
}