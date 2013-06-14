#include "../source/net/bitstream.h"
#include "../source/net/defines.h"
#include "../source/display/mapa.h"
#include "../../source/net/PjeManager.h"
#include "../../source/utilities/config_cliente.h"
#include "../../source/utilities/parser.h"
#include "../../source/utilities/console.h"
#include "../../source/utilities/aux_func.h"
#include "clientsocket.h"
#include "../../source/utilities/chatwindow.h"
#include "../source/utilities/bandera.h"
#include "../../source/utilities/lampara.h"
#include "../../source/utilities/mapaItem.h"
#include "../../source/utilities/zapatos.h"
#include "../../source/utilities/terremoto.h"
#include "../../source/utilities/escudo.h"
#include "../../source/utilities/interface.h"
#include "../../source/utilities/bolaDeCristal.h"
#include "../../source/utilities/GolemItem.h"
#include "../../source/utilities/Botella.h"
#include "../../source/utilities/Corazon.h"
#include "../../source/utilities/Hielo.h"
#include "../source/utilities/armaBomba.h"
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
extern double init_vel;
extern float init_radio;
extern bool init_bolaDeCristal, init_golem;
extern char init_energia,init_magia,init_escudo,init_terremoto,init_hielo,init_bombas;
extern config_general configuracion;
extern ResMan resman;
extern Console consola;
extern ChatWindow chat_window;
extern int estadoMovimiento;
extern std::vector<EntidadFija*> entidades_cargadas;
extern bool initAttCargados;

extern bool enAtaque; // indica si se encuentra en ataque
extern bool calcularAtaque; // indica si se debe calcular el camino minimo para un ataque
extern Personaje* personajeObjetivo;

bool ClientSocket::WSinit = false;
size_t ClientSocket::ref_count = 0;
ClientSocket::ClientSocket() {
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

bool ClientSocket::init(CRITICAL_SECTION* main_cs) {
	// CS stuff
	this->main_cs = main_cs;
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
	/*if(setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&val_true, sizeof(BOOL)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS TCP_NODELAY\n";
	}
	//if(setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(int)) != 0) {
		//std::cerr << "ERROR SETTING SOCK OPTIONS SO_RCVBUF\n";
	//}
	if(setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(int)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS SO_SNDBUF\n";
	}*/
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
		if(packet_size == -1 && tmp.size() >= 2) {
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
			std::stringstream ss;
			std::cout << "Server says: " << msg << "\n";
			ss << msg;
			consola.log(ss.str());
		}else if(pt == PROTO::CHAT) {
			std::string nick_source, nick_destino, mensaje;
			bs >> nick_source >> nick_destino >> mensaje;
			if(nick_source == "SERVER" && nick_destino == "SERVER") {
				chat_window.addMsgRaw(mensaje);
			}else{
				if(nick_source != pjm.getPjeLocal().getNick()) {
					chat_window.setNickDestino(nick_source);	
				}
				chat_window.addMsg(nick_source, mensaje);
			}
			chat_window.open();
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
				std::cout << "Receiving " << local_file;
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
						std::cout << "." << std::flush;
					}else if(pt == PROTO::FILE_DONE) {
						f.close();
						std::cout << "Ok!\n";
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
		}else if(pt == PROTO::OLD_ATT) {
			bool bolaDeCristal, golem;
			float recv_vel,radio;
			char energia,magia,energiaEscudo,cantTerremoto,cantHielo,cantBombas;
			bs >> recv_vel >> energia >> magia >> energiaEscudo >> cantTerremoto >> cantHielo >> radio >> bolaDeCristal >> golem >> cantBombas; 
			init_vel=(double)recv_vel;
			init_energia=energia;
			init_magia=magia;
			init_escudo=energiaEscudo;
			init_terremoto=cantTerremoto;
			init_hielo=cantHielo;
			init_radio=radio;
			init_bolaDeCristal = bolaDeCristal;
			init_golem = golem;
			init_bombas = cantBombas;
			initAttCargados = true;
			cout << "Velocidad restaurada: " << init_vel << endl;
		}else if(pt == PROTO::ESC_ID) {
			bs >> escenario_elegido_id;
			//std::cout << "RECEIVED ESC ID: (" << escenario_elegido_id << ")\n";
		}else if(pt == PROTO::NIEBLA_LIST) {
			// Esperamos a que cargue el mapa

			std::cout << "recibo tile explorado ";

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
		}else if(pt == PROTO::NIEBLA_SYNC) {
			// Esperamos a que cargue el mapa
			while(!cargoMapa) {
				Sleep(10);
			}
			std::string nick;
			bs >> nick;
			//busco el personaje de ese nick
			Personaje personaje = pjm.getPje(nick);

			short tile_list_size;
			bs >> tile_list_size;
			//std::cout << "RECEIVED NIEBLA_SYNC (" << tile_list_size << "): \n";
			for(int i = 0;i < tile_list_size;i++) {
				short x, y;
				bs >> x >> y;

				Tile::setearExplorados(x, y, &personaje, &mapa);
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
			// Si no existe el personaje, lo creamos
			if(!pjm.PjeExiste(new_nick)) {
				pjm.addPje(new_nick);
				auto& p = pjm.getPje(new_nick);
				// Agrega el personaje
				p.init(new_nick, new_type , 1 , 1 , 50 , 5, 100, 100 ,	configuracion.get_vel_personaje(),	0 , 50 , NULL , resman , Imagen::COLOR_KEY);
				// Posiciono el personaje
				mapa.getTile(x, y)->addEntidad(&p);
				p.setTileActual(mapa.getTile(x, y));
				p.freezar(!is_on);
			}else{
				// Si existe, lo marcamos como color
				auto& p = pjm.getPje(new_nick);
				p.freezar(!is_on);
			}
		}else if(pt == PROTO::INIT_ATT) {
			// Esperamos a que cargue el mapa
			while(!cargoMapa) {
				Sleep(10);
			}
			std::string nick_who;
			float vel_recv,radio;
			bool bolaDeCristal, golem;
			char energia,magia,energiaEscudo,cantTerremoto,cantHielo;
			bs >> nick_who >> vel_recv >> energia >> magia >> energiaEscudo >> cantTerremoto >> cantHielo >> radio >> bolaDeCristal >> golem;
			double vel=(double)vel_recv;
			//Seteamos los atributos del jugador
			pjm.getPje(nick_who).setVelocidad(vel);
			pjm.getPje(nick_who).setEnergia(energia);
			pjm.getPje(nick_who).setMagia(magia);
			pjm.getPje(nick_who).setEnergiaEscudo(energiaEscudo);
			pjm.getPje(nick_who).setTerremoto(cantTerremoto);
			pjm.getPje(nick_who).setHielo(cantHielo);
			pjm.getPje(nick_who).setRadio(radio);
			pjm.getPje(nick_who).setBolaDeCristal(bolaDeCristal);
			pjm.getPje(nick_who).setGolem(golem);
			cout << "Vel de player " << nick_who << " es " << vel << endl;
		}else if(pt == PROTO::PLAYER_EXIT) {
			// Esperamos a que cargue el mapa
			while(!cargoMapa) {
				Sleep(10);
			}
			std::string who_nick;
			bs >> who_nick;
			//std::cout << "RECEIVED PLAYER_EXIT: " << who_nick << "\n";
			// Si no existe el personaje, error
			if(!pjm.PjeExiste(who_nick)) {
				std::cerr << "Error @ PLAYER_EXIT: " << who_nick << " not found\n";
			}else{
				// Si existe, lo marcamos como desconectado
				auto& p = pjm.getPje(who_nick);
				p.freezar(true);
				std::stringstream ss;
				ss << who_nick << " abandono el servidor";
				consola.log(ss.str());
			}
		}else if(pt == PROTO::ATACAR) {
			std::string who_nick;
			bs >> who_nick;
			// Si no existe el personaje, error
			if(!pjm.PjeExiste(who_nick)) {
				std::cerr << "Error @ ATACAR: " << who_nick << " not found\n";
			}else{
				// Si existe, lo marcamos como desconectado
				auto& p = pjm.getPje(who_nick);
				p.ataque(NULL, &mapa);
			}
		}else if(pt == PROTO::DEFENDER) {
			std::string who_nick;
			bs >> who_nick;
			// Si no existe el personaje, error
			if(!pjm.PjeExiste(who_nick)) {
				std::cerr << "Error @ ATACAR: " << who_nick << " not found\n";
			}else{
				// Si existe, lo marcamos como desconectado
				auto& p = pjm.getPje(who_nick);
				p.defender(NULL, &mapa);
			}
		}else if(pt == PROTO::POS_REQUEST_REPLY) {
			bool reply;
			bs >> reply;
			if(estadoMovimiento != MOV::ESPERANDO_OK) {
				std::cout << "Received POS_REQUEST_REPLY without MOV::ESPERANDO_OK\n";
			}else{
				if(reply) {
					estadoMovimiento = MOV::OK_RECV;
					//std::cout << "GOT OK FROM SERVER\n";
				}else{
					estadoMovimiento = MOV::FAIL_RECV;
					std::cout << "GOT FAIL FROM SERVER\n";
				}
			}
		}else if(pt == PROTO::POS_REQUEST_REV_REPLY) {
			bool reply;
			bs >> reply;
			if(estadoMovimiento != MOV::ESPERANDO_OK) {
				std::cout << "Received POS_REQUEST_REV_REPLY without MOV::ESPERANDO_OK\n";
			}else{
				if(reply) {
					pjm.getPjeLocal().revivir();
					estadoMovimiento = MOV::OK_REV_RECV;
					std::cout << "GOT OK REV FROM SERVER\n";					
				}else{
					estadoMovimiento = MOV::FAIL_RECV;
					std::cout << "GOT FAIL REV FROM SERVER\n";
				}
			}
		}else if(pt == PROTO::MOVE_PLAYER) {
			std::string nick;
			int x, y;
			bs >> nick >> x >> y;
			if(!pjm.PjeExiste(nick)) {
				std::cout << "Server requested move of invalid PJ: " << nick << "\n";
			}else{
				auto& p = pjm.getPje(nick);
				p.mover(mapa.getTile(x, y));
				p.set_posicion_actualizada(false);
				if(enAtaque && &p == personajeObjetivo) {
					calcularAtaque = true;
				}
				if(pjm.getPjeLocal().getBolaDeCristal()){
					std::vector<Tile*> exploradosEnemigo = p.getTilesExplorados();
					for(auto it = exploradosEnemigo.begin(); it != exploradosEnemigo.end(); ++it){
						Tile* tileExplorado = mapa.getTile((*it)->getU(), (*it)->getV());
						pjm.getPjeLocal().agregarTilesExplorados(tileExplorado);
					}
				}
				//std::cout << "Server requested move of <" << nick << "> to " << x << ";" << y << "\n";
			}
		}else if(pt == PROTO::REV_PLAYER) {
			std::string nick;
			int x, y;
			bs >> nick >> x >> y;
			if(!pjm.PjeExiste(nick)) {
				std::cout << "Server requested move of invalid PJ: " << nick << "\n";
			}else{
				auto& p = pjm.getPje(nick);
				int posViejaU = p.getPosicion(&mapa)->getU();
				int posViejaV = p.getPosicion(&mapa)->getV();
				mapa.getTile(p.getPosicion(&mapa)->getU(), p.getPosicion(&mapa)->getV())->deleteEntidad(&p);
				mapa.getTile(x, y)->addEntidad(&p);
				p.setTileActual(mapa.getTile(x, y));
				p.revivir();

				std::cout << "Server requested revival of <" << nick << "> to " << x << ";" << y << "\n";
			}
		}else if(pt == PROTO::USE_ITEM) {
			char item;
			int posBombaX,posBombaY; //solo si es una bomba
			bs >> item;
			std::string nick_who;
			bs >> nick_who;
			if (item==ITEM::BOMBA) {
				bs >> posBombaX >> posBombaY;
				//Coloca la bomba en el mapa
				std::cout << "Colocando bomba en pos (" << posBombaX << "," << posBombaY << ")" << endl;
				ArmaBomba* bomba;
				bomba = new ArmaBomba("bomba",1,1,true, posBombaX , posBombaY,NULL,&mapa,resman,Imagen::COLOR_KEY );
				mapa.getTile(posBombaX, posBombaY)->addEntidad(bomba,&mapa);
				entidades_cargadas.push_back(bomba);
				pjm.getPje(nick_who).setBombaColocada(bomba);
				pjm.getPje(nick_who).setBombaX(posBombaX);
				pjm.getPje(nick_who).setBombaY(posBombaY);
			}
			// Hacemos algo, animaciones or something
		}else if(pt == PROTO::LEAVE_ITEM) {
			char tipoItem;
			int posItemX,posItemY;
			bs >> tipoItem >> posItemX >> posItemY;
			//Coloca el item en el mapa
	
			while(!cargoMapa) {
				Sleep(10);
			}

			Item* item;
			//case de todos los items y constructor
			switch(tipoItem){
				case(ITEM::LAMPARA):{
					item = new Lampara ("cofre",1,1,true, posItemX ,posItemY,NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case(ITEM::MAPAITEM):{
					item = new MapaItem ("cofre",1,1,true, posItemX ,posItemY,NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case(ITEM::ZAPATOS):{
					item = new Zapatos ("cofre",1,1,true, posItemX ,posItemY,NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::TERREMOTO):{
					item = new Terremoto ("cofre",1,1,true, posItemX , posItemY, NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::ESCUDO):{
					item = new Escudo ("cofre",1,1,true, posItemX , posItemY, NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::BOLA_DE_CRISTAL):{
					item = new BolaDeCristal("cofre",1,1,true, posItemX , posItemY, NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::GOLEM):{
					item = new GolemItem ("cofre",1,1,true, posItemX ,posItemY,NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::BOTELLA):{
					item = new Botella ("cofre",1,1,true, posItemX , posItemX, NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::CORAZON):{
					item = new Corazon ("cofre",1,1,true, posItemX , posItemY, NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
				case (ITEM::HIELO):{
					item = new Hielo ("cofre",1,1,true, posItemX , posItemY, NULL,&mapa,resman,Imagen::COLOR_KEY );
					mapa.getTile(posItemX, posItemY)->addEntidad(item,&mapa);
					entidades_cargadas.push_back(item);
					break;
				}
   				/*case (ITEM::BOMBA):{
					ArmaBomba cofre("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				}*/

			}
			//mapa.getTile(posItemX, posItemY)->addEntidad(&cofre,&mapa);
			//entidades_cargadas.push_back(&cofre);
			// Hacemos algo, animaciones or something
		}else if(pt == PROTO::BOMB_OFF) {
			std::string nick_who;
			bs >> nick_who;
			auto &p = pjm.getPje(nick_who);
			std::cout << "Exploto la bomba de " << nick_who << " en pos (" << p.getBombaX() << "," << p.getBombaY() << ")...";
			//Elimino bomba del mapa(FIX)
			mapa.getTile(p.getBombaX(),p.getBombaY())->deleteEntidad(p.getBombaColocada());
			std::cout << "eliminada" << endl;
		}else if(pt == PROTO::ITEM_OFF) {
			int x,y;
			bs >> x >> y;
			//Elimino item del mapa(FIX)
			Tile* tile = mapa.getTile(x,y);
			Entidad* entidadEliminada = NULL;
			std::cout << "item off " << x << " , " << y << "\n";
			std::vector<Entidad*> entidades = tile->getEntidades();
			for (auto it = entidades.begin(); it != entidades.end(); ++it){
				if((*it)->isItem()){
					entidadEliminada = (*it);
					break;
				}
			}
			tile->deleteEntidad(entidadEliminada);
		}else if(pt == PROTO::DAMAGE) {	
			std::string nick_who, nick_to;
			bs >> nick_who >> nick_to;
			char dmg;
			bs >> dmg;
			// Buscamos el personaje 
			if(nick_to == pjm.getPjeLocal().getNick()) {
				pjm.getPjeLocal().dañar(dmg);
				bs.clear();
				bs << PROTO::UPDATE_ATT << ATT::ENERGIA << pjm.getPjeLocal().getEnergia();
				this->send(bs.str());
				bs.clear();
				bs << PROTO::UPDATE_ATT << ATT::ENERGIA_ESCUDO << pjm.getPjeLocal().getEnergiaEscudo();
				this->send(bs.str());
			}else{
				for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
					if(it->first == nick_to) {
						it->second.dañar(dmg);
						break;
					}
				}
			}
		}else if(pt == PROTO::CONGELAR) {	
			std::string nick_who, nick_to;
			bs >> nick_who >> nick_to;
			// Buscamos el personaje 
			if(nick_to == pjm.getPjeLocal().getNick()) {
				pjm.getPjeLocal().freezar();
			}else{
				for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
					if(it->first == nick_to) {
						it->second.freezar();
						break;
					}
				}
			}
		}else if(pt == PROTO::DESCONGELAR) {	
			std::string nick_to;
			bs >> nick_to;
			// Buscamos el personaje 
			if(nick_to == pjm.getPjeLocal().getNick()) {
				pjm.getPjeLocal().freezar(false);
			}else{
				for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
					if(it->first == nick_to) {
						it->second.freezar(false);
						break;
					}
				}
			}
		}else if(pt == PROTO::UPDATE_ATT) {	
			char tipoAtt;
			bs >> tipoAtt;
			std::string nick_who;
			bs >> nick_who;			
			std::cout << "CLIENT SOCKET UPDATE_ATT: " << nick_who << "\n";
			float nuevoVal;
			char nuevoValor;
			bool nuevoValorBool;
			if ((tipoAtt==ATT::VEL) || (tipoAtt==ATT::RADIO)) {
				// Valor float: velocidad/radio
				bs >> nuevoVal;
			} else if((tipoAtt==ATT::BOLA_DE_CRISTAL) || (tipoAtt==ATT::GOLEM)){
				// Valor bool: bolaDeCristal/golem
				bs >> nuevoValorBool;
			} else {
				// Valor char: energia/magia/escudo/terremoto/hielo
				bs >> nuevoValor;
			}
			// Buscamos el personaje 
			for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
				if(it->first == nick_who) {
					if (tipoAtt==ATT::VEL) {
						it->second.setVelocidad((double)nuevoVal);
					} else if (tipoAtt==ATT::ENERGIA) {
						it->second.setEnergia(nuevoValor);
					} else if (tipoAtt==ATT::MAGIA) {
						it->second.setMagia(nuevoValor);
					} else if (tipoAtt==ATT::ENERGIA_ESCUDO) {
						it->second.setEnergiaEscudo(nuevoValor);
					} else if (tipoAtt==ATT::CANT_TERREMOTO) {
						it->second.setTerremoto(nuevoValor);
					} else if (tipoAtt==ATT::CANT_HIELO) {
						it->second.setHielo(nuevoValor);
					} else if (tipoAtt==ATT::BOLA_DE_CRISTAL) {
						it->second.setBolaDeCristal(nuevoValorBool);
					} else if (tipoAtt==ATT::GOLEM) {
						it->second.setGolem(nuevoValorBool);
					} else if (tipoAtt==ATT::RADIO) {
						it->second.setRadio(nuevoVal);
					}
					break;
				}
			}
		}else if(pt == PROTO::DEF_ATT) {	
			std::string nick_who;
			bs >> nick_who;
			float vel,radio;
			char energia,magia,escudo,terremoto,hielo,bombas;
			bool bolaCristal,golem;
			bs >> vel >> energia >> magia >> escudo >> terremoto >> hielo >> radio >> bolaCristal >> golem >> bombas;
			// Buscamos el personaje 
			cout << "Update def att de " << nick_who << " a " << vel;
			for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
				if(it->first == nick_who) {
					auto&p = pjm.getPje(nick_who);
					p.setVelocidad((double)vel);
					p.setEnergia(energia);
					p.setMagia(magia);
					p.setEnergiaEscudo(escudo);
					p.setTerremoto(terremoto);
					p.setHielo(hielo);
					p.setRadio(radio);
					p.setBolaDeCristal(bolaCristal);
					p.setGolem(golem);
					p.setCantBombas(bombas);
					break;
				}
			}
		}else if(pt == PROTO::NEW_FLAG) {	
			int x,y;
			bs >> x >> y;
			//Agrego bandera
			std::cout << "Adding flag in pos (" << x << "," << y << ")" << endl;
			// Esperamos a que cargue el mapa
			while(!cargoMapa) {
				Sleep(10);
			}
			Bandera* bandera;
			Tile* tile = mapa.getTile(x,y);
			bandera = new Bandera("bandera",1,1,true, -30 , 15,tile,&mapa,resman,Imagen::COLOR_KEY );
			mapa.getTile(x, y)->addEntidad(bandera,&mapa);
			entidades_cargadas.push_back(bandera);	
		}else if(pt == PROTO::WINNER) {	
			std::string nick_winner;
			bs >> nick_winner;
			// Veo si soy yo
			std::stringstream ss;
			if (pjm.getPjeLocal().getNick()==nick_winner) {
				ss << "Felicitaciones! Ganaste la mision!";
			} else {
				ss << nick_winner << " gano esta partida, jugale una revancha!";
			}
			consola.log(ss.str());
		}else if (pt == PROTO::ENEMY_DEAD){
			string EnemigoNick;
			//elimino enemigo
			bs >> EnemigoNick;
			for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
				if(it->first == EnemigoNick){
					mapa.getTilePorPixeles(it->second.getX(),it->second.getY())->deleteEntidad(&it->second);
					pjm.getPjes().erase(it);
					break;
				}
			}
				
		}else if (pt == PROTO::TRANSMUT){
			char tipo;
			bs >> tipo;
			std::string nick;
			bs >> nick;
			if (tipo == TIPO::LAPIDA) {
				if (pjm.getPjeLocal().getNick() == nick) {
					pjm.getPjeLocal().animacionMuerte();
				} else {
					pjm.getPje(nick).animacionMuerte();
				}				
				std::cout << "Convirtiendo a " << nick << " en lapida" << endl;
			} else if (tipo == TIPO::ESTRATEGIA_ENEMY) {
				std::cout << "Enemigo " << nick << "cambiando de estrategia" << endl;
			}				
		}else if (pt == PROTO::DESTRANSMUT){
			char tipo;
			bs >> tipo;
			std::string nick;
			bs >> nick;
			if (tipo == TIPO::LAPIDA) {
				if (pjm.getPjeLocal().getNick() == nick) {
					pjm.getPjeLocal().animacionRevivir();
				} else {
					pjm.getPje(nick).animacionRevivir();
				}				
				std::cout << "Desconvirtiendo a " << nick << " de lapida" << endl;
			} else if (tipo == TIPO::ESTRATEGIA_ENEMY) {
				std::cout << "Enemigo " << nick << "cambiando de estrategia" << endl;
			}				
		}else{
			std::cout << "Unknown packet type " << int(pt) << " received\n";
		}

		LeaveCriticalSection(this->main_cs);
	}
}

bool ClientSocket::isOpen() const {
	return this->open;
}