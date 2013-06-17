#include "serversocket.h"

#include "../source/utilities/aux_func.h"
#include "parserServer.h"
#include "misiones.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <ctime>

#include <Windows.h>
#include <process.h>

#include "playerman.h"
#include "mapaservidor.h"
#include "enemigoServer.h"
#include "golem.h"
#include "../source/utilities/timer.h"
extern PlayerManager pm;
extern MapaServidor mapa;
extern std::vector <config_entidad> entidades;
extern int escenario_elegido_id;
extern bool puedeMoverseEnemigo; 
extern bool conectandose;
bool ServerSocket::WSinit = false;
size_t ServerSocket::ref_count = 0;
const int tiempoAtaque = 1000;
BitStream bs;
extern Misiones mision;
extern bool crearMision;

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
	}/*
	//if(setsockopt(ListenSocket, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(int)) != 0) {
		//std::cerr << "ERROR SETTING SOCK OPTIONS SO_RCVBUF\n";
	//}
	if(setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(int)) != 0) {
		std::cerr << "ERROR SETTING SOCK OPTIONS SO_SNDBUF\n";
	}*/
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
    serverService.sin_addr.s_addr = htonl (INADDR_ANY);
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
		}/*
		//if(setsockopt(tmp_sck, SOL_SOCKET, SO_RCVBUF, (char*)&buff_size, sizeof(int)) != 0) {
			//std::cerr << "ERROR SETTING SOCK OPTIONS SO_RCVBUF\n";
		//}
		if(setsockopt(tmp_sck, SOL_SOCKET, SO_SNDBUF, (char*)&buff_size, sizeof(int)) != 0) {
			std::cerr << "ERROR SETTING SOCK OPTIONS SO_SNDBUF\n";
		}*/
		// Ingresamos la conexion en la tabla
		addClient(tmp_st, tmp_sck);
		// "Devolvemos" el id para la conexion que se acaba de aceptar
		clients_queue.push(buildId(tmp_st));
		// Spawneamos el thread
		_beginthreadex(NULL, 0, ServerSocket::acceptLastEntry, (void*)this, 0, NULL);
		return true;
	}
}

// Agrega un item al vector
void ServerSocket::addItem(unsigned char tipo, int x, int y){
	ItemServidor item;
	item.init(tipo, x, y);

	items.push_back(item);
}

// Saca un item del vector
void ServerSocket::removeItem(int x, int y){
	std::vector<ItemServidor> new_items;
	for(auto it = items.begin(); it != items.end(); ++it){
		if((it->getX() != x) || (it->getY() != y)){
			new_items.push_back(*it);			
		}
	}
	items = new_items;
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
	return getClient(cid).send(msg);
}

// Funcion de send
bool Client::send(const std::string& msg) {
	if(sock == SOCKET_ERROR) {
		std::cerr << "Cliente invalid pasado a send(): " << sock << "\n";
		return false;
	}

	BitStream bs;
	bs << msg;

	int res = ::send(sock, bs.str().c_str(), bs.str().size(), 0);
	if(res == SOCKET_ERROR) {
		std::cerr << "Error enviando mensaje: " << WSAGetLastError() << "\n";
		return false;
	}else{
		//std::cout << "Se enviaron " << res << " bytes: " << bs.str() << "\n";
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
	return getClient(cid).receive(buff);
}

// Funcion de receive
bool Client::receive(std::string& buff) {

	packet.clear();
	packet_size = -1;
	bytes_read = 0;
	// Loop until we read a full packet
	while(true) {
		tmp.clear();
		// If we have something in the queue, push it it
		if(queue_buf.size() > 0) {
			tmp.append(queue_buf);
			//std::cout << "APPENDING FROM QUEUE (" << c.queue_buf.size() << ")\n";
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
				//std::cout << "Got more: " << c.bytes_read << "," << c.packet_size << "-----------------------------------\n";
				// Trim and store
				//std::cout << "Delta: " << c.bytes_read - c.packet_size << "\n";
				queue_buf = tmp.substr(tmp.size() - (bytes_read - packet_size));
				//std::cout << "STORING EXTRA (" << c.queue_buf.size() << ")\n";
				packet.append(tmp.substr(0, packet_size-sizeof(short)));
				//std::cout << "APPENDING " << c.packet_size << "\n";
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
		//std::cout << bytes_read << "==";
		//std::cout << "RECEIVING " << cid << "...";
		char recvbuf[DEFAULT_BUFLEN];
		int res = ::recv(sock, recvbuf, DEFAULT_BUFLEN, 0);
		//std::cout << "OK\n";
		//std::cout << bytes_read << "\n";
		if(res > 0) {
			//bytes_read += res;
			//std::cout << "Read " << res << " bytes\n";
			//std::cout << "BytesRead: " << c.bytes_read << "\n";
			queue_buf.append(recvbuf, res);
			//std::cout << "(" << queue_buf.size() << ") " << queue_buf << "\n";
		}else if(res == 0) {
			std::cout << "Connection closed\n";
			return false;
		}else{
			std::cout << "Receive error: " << WSAGetLastError() << "\n";
			return false;
		}
	}
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
		//actualizo estado de enemigos  y golem si no quedo ningun cliente
		if(clients_map.empty()){
			for(auto it=pm.getEnemies().begin();it!=pm.getEnemies().end();it++){
				it ->second->setPos(it->second->getXSiguiente(),it->second->getYSiguiente());
			}
			for(auto it=pm.getGolems().begin();it!=pm.getGolems().end();it++){
				it ->second->setPos(it->second->getXSiguiente(),it->second->getYSiguiente());
			}
		}
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
	EnterCriticalSection(&critSect);
	//bloqueo el loop ppal
	conectandose=true;
	// Iniciamos el seed de srand
	std::srand(std::time(NULL));

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
		// Verificamos si el nick está en uso
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
			//BitStream bs;
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
		pm.getPlayer(new_nick).setOnline();
		
		// Le mandamos la posicion inicial
		BitStream bs;
		bs << PROTO::INITPOS << pm.getPlayer(new_nick).getX() << pm.getPlayer(new_nick).getY();
		// al pedo: pm.getPlayer(new_nick).addTileRecorrido(pm.getPlayer(new_nick).getX(), pm.getPlayer(new_nick).getY());
		//std::cout << "SEND INIT POS (" << pm.getPlayer(new_nick).getX() << "," << pm.getPlayer(new_nick).getY() << ")\n";
		send(cid, bs.str());

		//Le mandamos la velocidad que tenia		
		bs.clear();
		auto& p=pm.getPlayer(new_nick); //alias shortcut
		bs << PROTO::OLD_ATT << (float)p.getVelocidad() << p.getEnergia() << p.getMagia() << p.getEnergiaEscudo() << p.getTerremoto() << p.getHielo() << (float)p.getRadio() << (bool)p.getBolaDeCristal() << (bool)p.tieneGolem() << p.getCantBombas(); 
		send(cid, bs.str());

		// Le mandamos el id escenario
		bs.clear();
		bs << PROTO::ESC_ID << escenario_elegido_id;
		//std::cout << "SEND ESC ID (" << escenario_elegido_id << ")\n";
		send(cid, bs.str());

		// Mandamos los tiles recorridos
		if(pm.getPlayer(new_nick).getTilesRecorridos().size() > 0){
			bs.clear();
			bs << PROTO::NIEBLA_LIST << short(pm.getPlayer(new_nick).getTilesRecorridos().size());
			for(auto it = pm.getPlayer(new_nick).getTilesRecorridos().begin(); it != pm.getPlayer(new_nick).getTilesRecorridos().end();it++) {
				bs << it->first << it->second;
			}
			send(cid, bs.str());
		}		

		// Para tipear menos, p_local = referencia al player de este thread
		const auto& p_local = pm.getPlayer(new_nick);

		// Avisamos a los otros jugadores del nuevo jugador
		for(auto it = clients_map.begin();it != clients_map.end();it++) {
			if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
			BitStream bs;
			
			bs << PROTO::NEW_PLAYER << new_nick << p_local.getTipo() << p_local.getX() << p_local.getY() << p_local.isOn();
			it->second.send(bs.str());
			//Mando los atributos principales del jugador
			bs.clear();
			auto& p=pm.getPlayer(new_nick);
			//cout << "Send a " << it->second.nick << " vel de " << new_nick << " " << p.getVelocidad() << endl;
			bs << PROTO::INIT_ATT << new_nick << (float)p.getVelocidad() << p.getEnergia() << p.getMagia() << p.getEnergiaEscudo() << p.getTerremoto() << p.getHielo() << (float)p.getRadio() << (bool)p.getBolaDeCristal() << (bool)p.tieneGolem() << p.tieneTransmut();
			it->second.send(bs.str());
			if (pm.getPlayer(new_nick).isCongelado()) {
				bs.clear();
				bs << PROTO::CONGELAR << std::string("RESTORE") << new_nick;
				it->second.send(bs.str());
			}
		}

		// Mandamos todos los otros players al que se unio
		for(auto it = pm.getPlayers().begin();it != pm.getPlayers().end();it++) {
			if(it->second.getNick() == new_nick) continue; // Salteamos a nuestro jugador			
			auto& p = pm.getPlayer(it->second.getNick());
			bs.clear();
			bs << PROTO::NEW_PLAYER << it->second.getNick() << p.getTipo() << p.getX() << p.getY() << p.isOn();
			send(cid, bs.str());
			//Mando los atributos principales del jugador
			bs.clear();
			bs << PROTO::INIT_ATT << it->first << (float)p.getVelocidad() << p.getEnergia() << p.getMagia() << p.getEnergiaEscudo() << p.getTerremoto() << p.getHielo() << (float)p.getRadio() << (bool)p.getBolaDeCristal() << (bool)p.tieneGolem() << p.tieneTransmut();
			send(cid,bs.str());
		}
		// Mandamos todos los enemigos al que se unio
		for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end();it++) {
			Enemigo* unEnemigo = it->second;
	
			if( unEnemigo->getNick() == new_nick) continue; // Salteamos a nuestro jugador			
			auto p = pm.getEnemy(unEnemigo->getNick());
			bs.clear();
			bs << PROTO::NEW_PLAYER << unEnemigo->getNick() << unEnemigo->getTipo() << unEnemigo->getX() << unEnemigo->getY() << unEnemigo-> isOn();
			send(cid, bs.str());
			//Mando los atributos principales del jugador
			bs.clear();
			bs << PROTO::INIT_ATT << it->first << (float)unEnemigo->getVelocidad() << unEnemigo->getEnergia() << unEnemigo->getMagia() << unEnemigo->getEnergiaEscudo() << unEnemigo->getTerremoto() << unEnemigo->getHielo() << (float)unEnemigo->getRadio() << (bool)unEnemigo->getBolaDeCristal() << (bool)unEnemigo->tieneGolem() << unEnemigo->tieneTransmut();
			send(cid,bs.str());
		}
		// Mandamos todos los Golems al que se unio
		for(auto it = pm.getGolems().begin();it != pm.getGolems().end();it++) {
			Golem* unGolem = it->second;
	
			if( unGolem->getNick() == new_nick) continue; // Salteamos a nuestro jugador			
			auto p = pm.getEnemy(unGolem->getNick());
			bs.clear();
			bs << PROTO::NEW_PLAYER << unGolem->getNick() << unGolem->getTipo() << unGolem->getX() << unGolem->getY() << unGolem-> isOn();
			send(cid, bs.str());
			//Mando los atributos principales del jugador
			bs.clear();
			bs << PROTO::INIT_ATT << it->first << (float)unGolem->getVelocidad() << unGolem->getEnergia() << unGolem->getMagia() << unGolem->getEnergiaEscudo() << unGolem->getTerremoto() << unGolem->getHielo() << (float)unGolem->getRadio() << (bool)unGolem->getBolaDeCristal() << (bool)unGolem->tieneGolem() << unGolem->tieneTransmut();
			send(cid,bs.str());
		}

		//Le aviso todos los que estaban congelados
		for (auto it = pm.getPlayers().begin();it != pm.getPlayers().end();it++) {
			if (it->second.isCongelado()) {
				bs.clear();
				bs << PROTO::CONGELAR << std::string("RESTORE") << it->first;
				send(cid,bs.str());
			}
		}
		//Mandamos las banderas si correponde la mision
		if (mision.getTipo()==Misiones::MISION_BANDERAS) {
			int contBanderas=0;
			std::list<std::pair<int,int>> banderas=mision.getBanderas();
			for (auto it=banderas.begin(); it != banderas.end(); it++) {
				bs.clear();
				bs << PROTO::NEW_FLAG << it->first << it->second;
				send(cid,bs.str());
				std::cout << "Sending flag " << contBanderas+1 << " pos (" << it->first << "," << it->second << ")" << endl;
				contBanderas++;
			}			
		}

		//Mandamos los items que habia en el mapa
		for (auto it=items.begin(); it != items.end(); it++) {
			bs.clear();
			bs << PROTO::LEAVE_ITEM << it->getTipo() << it->getX() << it->getY();
			send(cid,bs.str());
			std::cout << "Sending item " << it->getTipo() << " pos (" << it->getX() << "," << it->getY() << ")" << endl;
		}			

		LeaveCriticalSection(&critSect);
		//desbloqueo el loop
		conectandose = false;

		//Objetivos de las misiones
		if (crearMision) {
			if (mision.getTipo()==Misiones::MISION_BANDERAS) {
				Sleep(2000);
				bs.clear();
				bs << PROTO::TEXTMSG << std::string("Captura todas las banderas...");
				send(cid,bs.str());
				bs.clear();
				std::stringstream msj;
				msj << "Hay " << mision.cantBanderas() << " en total!";
				bs << PROTO::TEXTMSG << msj.str();
				send(cid,bs.str());
			} else {
				Sleep(2000);
				bs.clear();
				bs << PROTO::TEXTMSG << std::string("Elimina a tu eterno enemigo!");
				send(cid,bs.str());
				bs.clear();
				bs << PROTO::TEXTMSG << std::string("Quiero ver sangre!!!");
				send(cid,bs.str());
			}
		}

		// Receive loop
		while(this->receive(cid, buff)) {
			Sleep(50);
			EnterCriticalSection(&critSect);

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
				// Leemos para saber a quien mandar
				std::string nick_destino, nick_source, mensaje;
				bs >> nick_source >> nick_destino >> mensaje;
				bs.clear();
				bs << PROTO::CHAT << nick_source << nick_destino << mensaje;
				int found_count = 0;
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == nick_source || it->second.nick == nick_destino) {
						it->second.send(bs.str());
						found_count++;
					}
				}
				// No se encontro el destino (calculo)
				if(found_count < 2) {
					bs.clear();
					bs << PROTO::CHAT << std::string("SERVER") << std::string("SERVER") << std::string("Player unavailable");
					send(cid, bs.str());
				}
			}else if(pt == PROTO::NIEBLA_SYNC) {
				int new_tile_x, new_tile_y;
				bs >> new_tile_x >> new_tile_y;
				pm.getPlayer(new_nick).addTileRecorrido(new_tile_x, new_tile_y);

				if(!(pm.getPlayer(new_nick).getSeMovio())){
					//recorro los personajes y les aviso q explore este tile
					for(auto it = pm.getPlayers().begin(); it != pm.getPlayers().end(); ++it){
						if(it->first == new_nick) continue;
						if(it->second.getBolaDeCristal()){
							if(!(it->second.existsTileRecorrido(new_tile_x, new_tile_y))) {
								it->second.addTileRecorrido(new_tile_x, new_tile_y);
								bs.clear();
								bs << PROTO::NIEBLA_LIST << (short)1;
								bs << (short)new_tile_x << (short)new_tile_y;
								send(getCIDbyNick(it->first), bs.str());
							}
						}
					}				
				}

				//std::cout << "RECEIVED NIEBLA SYNC: " << new_tile_x << "," << new_tile_y << "\n";
			}else if(pt == PROTO::ATACAR) {
				//std::cout << "RECV: ATACAR\n";
				std::string nick_who,nick_to;
				bs >> nick_who >> nick_to;
				// Avisamos a los otros jugadores del nuevo jugador
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					BitStream bs;
					bs << PROTO::ATACAR << nick_who << nick_to;
					it->second.send(bs.str());
				}
			}else if(pt == PROTO::DEFENDER) {
				std::string nick_atacante;
				bs >> nick_atacante;
				// Avisamos a los otros jugadores del nuevo jugador
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					BitStream bs;
					bs << PROTO::DEFENDER << nick_atacante;
					it->second.send(bs.str());
				}
			}else if(pt == PROTO::CHANGE_DIR) {
				unsigned int dirActual;
				bs >> dirActual;
				// Avisamos a los otros jugadores del nuevo jugador
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					BitStream bs;
					bs << PROTO::CHANGE_DIR << new_nick << dirActual;
					it->second.send(bs.str());
				}
			}else if(pt == PROTO::USE_ITEM) {
				std::string nick_who;
				bs >> nick_who;
				char item;
				bs >> item;
				int posBombaX,posBombaY; //solo si recibo una bomba
				std::cout << "uso item \n";
				if (item==ITEM::TERREMOTO) pm.getPlayer(nick_who).restarTerremoto();
				if (item==ITEM::HIELO) pm.getPlayer(nick_who).restarHielo();
				if (item==ITEM::BOMBA) {
					bs >> posBombaX >> posBombaY;
					std::cout << nick_who << "puso bomba en pos (" << posBombaX << "," << posBombaY << ")" << endl;
				}
				// Avisamos a los otros jugadores 
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					bs.clear();
					bs << PROTO::USE_ITEM << item << nick_who;
					//Si es bomba ademas paso la posicion donde se la coloco
					if (item==ITEM::BOMBA) {
						bs << posBombaX << posBombaY;
						std::cout << "Update a " << it->second.nick << " bomba en pos (" << posBombaX << "," << posBombaY << ")" << endl;
					}
					it->second.send(bs.str());
				}
			}else if(pt == PROTO::LEAVE_ITEM) {
				char item;
				int posItemX,posItemY;
				bs >> item >> posItemX >> posItemY;
				// Avisamos a los otros jugadores 
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					bs.clear();
					bs << PROTO::LEAVE_ITEM << item << posItemX << posItemY;
					it->second.send(bs.str());
					std::cout << "Update a " << it->second.nick << " item en pos (" << posItemX << "," << posItemY << ")" << endl;
				}
				addItem(item, posItemX, posItemY);
			}else if(pt == PROTO::BOMB_OFF) {
				// Avisamos a los otros jugadores 
				std::cout << "exploto \n";
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					bs.clear();
					bs << PROTO::BOMB_OFF << new_nick;
					it->second.send(bs.str());
					std::cout << "Update a " << it->second.nick << ": bomba explotada de " << new_nick <<endl;
				}
			}else if(pt == PROTO::ITEM_OFF) {
				// Avisamos a los otros jugadores 
				int x, y;
				bs >> x >> y;
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador
					bs.clear();
					bs << PROTO::ITEM_OFF << x << y;
					it->second.send(bs.str());
					std::cout << " agarro item en pos " << x << y << "\n";
				}
				removeItem(x,y);
			}else if(pt == PROTO::DAMAGE) {	
				std::string nick_who, nick_to;
				bs >> nick_who >> nick_to;
				char dmg;
				bs >> dmg;
				std::cout << "Ataque " << nick_who << "->" << nick_to << " de " << (int)dmg << endl;
				//Actualizamos datos locales para la mision de matar un enemigo
				//pm.getPlayer(nick_to).atacadoPor(nick_who);
				bool terminoMision = false;
				bool murioPersonaje = false;
				//busco si se ataco a un enemigo
				for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end();it++) {
					if(it->second->getNick() == nick_to){
						cout << "Se ataco a enemigo " << nick_to  << " con danio" << (int)dmg << endl;
						it->second->hacerDanio(dmg);
						it->second->atacadoPor(nick_who);
						if (!it->second->estaVivo()){
							murioPersonaje = true;
							cout << "Murio enemigo " << nick_to << endl;
							if(mision.getTipo() == Misiones::MISION_ENEMIGO){
								if (mision.enemigoMision() == it->second->getNick()){
									//termino mision
									terminoMision = true;
								}
							}
							cout << "elimine a " << it->second->getNick()<<endl;
							pm.getEnemies().erase(it);
							break;
						}
					}
				}
				//golem
				for(auto it = pm.getGolems().begin();it != pm.getGolems().end();it++) {
					if(it->second->getNick() == nick_to){
						it->second->hacerDanio(dmg);
						it->second->atacadoPor(nick_who);
						if (!it->second->estaVivo()){
							murioPersonaje = true;
							cout << "elimine a " << it->second->getNick()<<endl;
							pm.getGolems().erase(it);
							break;
						}
					}
				}
				// Avisamos a los otros jugadores 
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador de avisarle
					bs.clear();
					bs << PROTO::DAMAGE << nick_who << nick_to << dmg;
					it->second.send(bs.str());
				}
				//aviso si murio enemigo o termine mision
				if (murioPersonaje){
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						bs.clear();
						bs << PROTO::ENEMY_DEAD << nick_to;
						it->second.send(bs.str());
						if(terminoMision){
							//aviso a los demas que termino la mision
							bs.clear();
							bs << PROTO::WINNER << nick_who;							
							it->second.send(bs.str());
						}
					}
					if (terminoMision) Sleep(5000);
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						if(terminoMision){
							for (auto itP = pm.getPlayers().begin(); itP != pm.getPlayers().end(); itP++) {
								bs.clear();
								bs << PROTO::RESET_PLAYER << itP->first << itP->second.getXInicial() << itP->second.getYInicial();
								it->second.send(bs.str());
								//std::cout << "Mandando reset de " << itP->first << " de la mision a " << it->second.nick << "\n";
								itP->second.reiniciar();
							}
							for (auto itE = pm.getEnemies().begin(); itE != pm.getEnemies().end(); itE++) {
								delete (itE->second);
							}
							pm.getEnemies().clear();
							//Agregar enemigos automaticos
							pm.addEnemy("EnemigoMaster","soldado",mapa,1);
							//pm.addEnemy("Enemigo2","soldado",mapa,2);	
							//pm.addEnemy("Enemigo3","soldado",mapa,2);
							//Mato a todos los golems que quedaron
							for (auto itG = pm.getGolems().begin(); itG != pm.getGolems().end(); itG++) {
								delete (itG->second);
							}
							pm.getGolems().clear();	
							for(auto it = clients_map.begin();it != clients_map.end();it++) {
								for (auto itE = pm.getEnemies().begin(); itE != pm.getEnemies().end(); itE++) {							
									bs.clear();
									bs << PROTO::NEW_PLAYER << itE->second->getNick() << itE->second->getTipo() << itE->second->getX() << itE->second->getY() << itE->second-> isOn();
									it->second.send(bs.str());
									//Mando los atributos principales del enemigo
									bs.clear();
									bs << PROTO::INIT_ATT << itE->second->getNick() << (float)itE->second->getVelocidad() << itE->second->getEnergia() << itE->second->getMagia() << itE->second->getEnergiaEscudo() << itE->second->getTerremoto() << itE->second->getHielo() << (float)itE->second->getRadio();
									it->second.send(bs.str());
								}
							}
						}
					}
				}
			}else if(pt == PROTO::CONGELAR) {	
				std::string nick_who, nick_to;
				bs >> nick_who >> nick_to;
				// Actualizamos el estado del congelado
				if (pm.enemyExists(nick_to)) {
					pm.getEnemy(nick_to)->congelar();
				} else if (pm.golemExists(nick_to)) {
					pm.getGolem(nick_to)->congelar();
				} else {
					pm.getPlayer(nick_to).congelar();
				}
				// Avisamos a los otros jugadores 
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador de avisarle
					bs.clear();
					bs << PROTO::CONGELAR << nick_who << nick_to;
					it->second.send(bs.str());
				}
			}else if(pt == PROTO::DESCONGELAR) {	
				std::string nick_to;
				bs >> nick_to;
				// Actualizamos el estado del congelado
				if (pm.enemyExists(nick_to)) {
					pm.getEnemy(nick_to)->descongelar();
				} else if (pm.golemExists(nick_to)) {
					pm.getGolem(nick_to)->descongelar();
				} else {
					pm.getPlayer(nick_to).descongelar();
				}
				// Avisamos a los jugadores que descongelen al jugador
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador de avisarle
					bs.clear();
					bs << PROTO::DESCONGELAR << nick_to;
					it->second.send(bs.str());
				}
			}else if(pt == PROTO::UPDATE_ATT) {	
				char tipoAtt;
				bs >> tipoAtt;
				float nuevoVal;
				char nuevoValor;
				bool nuevoValorBool;
				if ((tipoAtt==ATT::VEL) || (tipoAtt==ATT::RADIO)) {
					// Valor float: velocidad/radio
					bs >> nuevoVal;
					//cout << "att recv " << nuevoVal << " tipo " << (int)tipoAtt << endl;
				}else if((tipoAtt==ATT::BOLA_DE_CRISTAL) || (tipoAtt==ATT::GOLEM) || (tipoAtt==ATT::TRANSMUT)){
					//valor bool: bola de cristal/golem/transmutacion
					bs >> nuevoValorBool;
					//cout << "att recv " << nuevoValorBool << " tipo " << (int)tipoAtt << endl;
				} else {
					// Valor char: energia/magia/escudo/terremoto/hielo/radio/bombas
					bs >> nuevoValor;
					//cout << "att recv " << (int)nuevoValor << " tipo " << (int)tipoAtt << endl;
				}
				// Avisamos a los otros jugadores 
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) {
						if (tipoAtt==ATT::VEL) {
							pm.getPlayer(new_nick).setVelocidad((double)nuevoVal);
						} else if (tipoAtt==ATT::ENERGIA) {
							pm.getPlayer(new_nick).setEnergia(nuevoValor);
						} else if (tipoAtt==ATT::MAGIA) {
							pm.getPlayer(new_nick).setMagia(nuevoValor);
						} else if (tipoAtt==ATT::ENERGIA_ESCUDO) {
							pm.getPlayer(new_nick).setEnergiaEscudo(nuevoValor);
						} else if (tipoAtt==ATT::CANT_TERREMOTO) {
							pm.getPlayer(new_nick).setTerremoto(nuevoValor);
						} else if (tipoAtt==ATT::CANT_HIELO) {
							pm.getPlayer(new_nick).setHielo(nuevoValor);
						} else if (tipoAtt==ATT::BOLA_DE_CRISTAL) {
							pm.getPlayer(new_nick).setBolaDeCristal(nuevoValorBool);

							if(nuevoValorBool){
								cout << "mandando tiles de otros jugadores" << endl;;
								//mando al jugador los tiles del resto
								for(auto it = pm.getPlayers().begin(); it != pm.getPlayers().end();it++) {
									if(it->first == new_nick) continue;
									if(it->second.getTilesRecorridos().size() > 0) {
										bs.clear();
										bs << PROTO::NIEBLA_LIST << short(it->second.getTilesRecorridos().size());
										for(auto it2 = it->second.getTilesRecorridos().begin(); it2 != it->second.getTilesRecorridos().end();it2++) {
											bs << it2->first << it2->second;
											it->second.addTileRecorrido(it2->first, it2->second);
										}
										send(cid, bs.str());
									}
								}
							}
						} else if (tipoAtt==ATT::GOLEM) {
							pm.getPlayer(new_nick).setGolem(nuevoValorBool);
						} else if (tipoAtt==ATT::TRANSMUT) {
							pm.getPlayer(new_nick).setTransmut(nuevoValorBool);
						} else if (tipoAtt==ATT::RADIO) {
							pm.getPlayer(new_nick).setRadio(nuevoVal);
						} else if (tipoAtt==ATT::CANT_BOMBAS) {
							pm.getPlayer(new_nick).setCantBombas(nuevoValor);
						}

						continue; // Salteamos a nuestro jugador de avisarle
					}
					bs.clear();
					if ((tipoAtt==ATT::VEL) || (tipoAtt==ATT::RADIO)) {
						bs << PROTO::UPDATE_ATT << tipoAtt << new_nick << nuevoVal;
					} else if((tipoAtt==ATT::BOLA_DE_CRISTAL) || (tipoAtt==ATT::GOLEM) || (tipoAtt==ATT::TRANSMUT)){
						bs << PROTO::UPDATE_ATT << tipoAtt << new_nick << nuevoValorBool;					
					} else {
						bs << PROTO::UPDATE_ATT << tipoAtt << new_nick << nuevoValor;
					}
					it->second.send(bs.str());
					
				}
			}else if(pt == PROTO::DEF_ATT) {	
				float vel,radio;
				char energia,magia,escudo,terremoto,hielo,bombas;
				bool bolaCristal,golem,transmut;
				bs >> vel >> energia >> magia >> escudo >> terremoto >> hielo >> radio >> bolaCristal >> golem >> bombas >> transmut;
				//Actualizo localmente
				auto& p = pm.getPlayer(new_nick);
				p.setVelocidad((double)vel);
				p.setEnergia(energia);
				p.setMagia(magia);
				p.setEnergiaEscudo(escudo);
				p.setTerremoto(terremoto);
				p.setHielo(hielo);
				p.setRadio(radio);
				p.setBolaDeCristal(bolaCristal);
				if(bolaCristal){
					cout << "mandando tiles de otros jugadores" << endl;;
					//mando al jugador los tiles del resto
					for(auto it = pm.getPlayers().begin(); it != pm.getPlayers().end();it++) {
						if(it->first == new_nick) continue;
						if(it->second.getTilesRecorridos().size() > 0) {
							bs.clear();
							bs << PROTO::NIEBLA_LIST << short(it->second.getTilesRecorridos().size());
							for(auto it2 = it->second.getTilesRecorridos().begin(); it2 != it->second.getTilesRecorridos().end();it2++) {
								bs << it2->first << it2->second;
								it->second.addTileRecorrido(it2->first, it2->second);
							}
							send(cid, bs.str());
						}
					}
				}
				p.setGolem(golem);
				p.setCantBombas(bombas);
				p.setTransmut(transmut);
				// Avisamos a los otros jugadores 
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					if(it->second.nick == new_nick) continue; // Salteamos a nuestro jugador de avisarle
					bs.clear();
					bs << PROTO::DEF_ATT << new_nick << (float)p.getVelocidad() << p.getEnergia() << p.getMagia() << p.getEnergiaEscudo() << p.getTerremoto();
					bs << p.getHielo() << p.getRadio() << p.getBolaDeCristal() << p.tieneGolem() << p.getCantBombas() << p.tieneTransmut();
					it->second.send(bs.str());
					//cout << "Mandando def vel de " << new_nick << " a " << it->second.nick << " valor " << p.getVelocidad() << endl;
				}
			}else if(pt == PROTO::REQUEST_POS) {
				int x, y;
				bs >> x >> y;
				bool ok = true;
				std::cout << "GOT REQUEST_POS <" << new_nick << ">: " << x << ";" << y << "\n";
				// Iteramos para ver si hay algun personaje
				for(auto it = pm.getPlayers().begin();it != pm.getPlayers().end();it++) {
					if(it->second.getX() == x && it->second.getY() == y) {
						ok = false;
						break;
					}
					if(it->second.getXSiguiente() == x && it->second.getYSiguiente() == y) {
						//si se mueve a ahi
						ok = false;
						break;
					}
				}				
				//enemigos
				for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end();it++) {
					if(it->second->getX() == x && it->second->getY() == y) {
						//si esta ahi
						ok = false;
						break;
					}
					if(it->second->getXSiguiente() == x && it->second->getYSiguiente() == y) {
						//si se mueve a ahi
						ok = false;
						break;
					}
				}
				//golem
				for(auto it = pm.getGolems().begin();it != pm.getGolems().end();it++) {
					if(it->second->getX() == x && it->second->getY() == y) {
						//si esta ahi
						ok = false;
						break;
					}
					if(it->second->getXSiguiente() == x && it->second->getYSiguiente() == y) {
						//si se mueve a ahi
						ok = false;
						break;
					}
				}
				//Si esta congelado no se puede mover a ningun lado
				if (pm.getPlayer(new_nick).isCongelado()) {
					ok = false;
				}
				if(ok) {
					//le seteo el seMovio en true
					pm.getPlayer(new_nick).setSeMovio(true);
					// Si el movimiento esta ok, actualizamos la posicion
					mapa.actualizarGrafo(pm.getPlayer(new_nick).getX(),pm.getPlayer(new_nick).getY());// actualizo la posicion vieja
					pm.getPlayer(new_nick).setPos(x, y);
					mapa.actualizarGrafoPersonajes(pm); //actualizo la nueva posicion
					// Informamos al jugador que esta ok
					bs.clear();
					bs << PROTO::POS_REQUEST_REPLY << true;
					std::cout << "REPLY: OK\n";
					this->send(cid, bs.str());
					// Informamos a los demas del movimiento
					bs.clear();
					bs << PROTO::MOVE_PLAYER << new_nick << x << y;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						if(it->second.nick == new_nick) continue; // Salteamos el jugador en cuestion
						it->second.send(bs.str());
						std::cout << "Mandando update a " << it->second.nick << "\n";
						if(pm.getPlayer(it->second.nick).getBolaDeCristal()){
							//si tiene bola de cristal le mando los tiles explorados del que se movio para que los agregue
							bs.clear();
							bs << PROTO::NIEBLA_LIST << short(pm.getPlayer(new_nick).getTilesRecorridos().size());
							for(auto it2 = pm.getPlayer(new_nick).getTilesRecorridos().begin(); it2 != pm.getPlayer(new_nick).getTilesRecorridos().end();it2++) {
								bs << it2->first << it2->second;
								pm.getPlayer(it->second.nick).addTileRecorrido(it2->first, it2->second);
							}
							it->second.send(bs.str());
						}
					}
				}else{
					// Si fallo, avisamos al cliente
					bs.clear();
					bs << PROTO::POS_REQUEST_REPLY << false;
					std::cout << "REPLY: FAIL\n";
					this->send(cid, bs.str());
				}
			}else if(pt == PROTO::EN_MOVE_CMPLT){

				string nickPersonajeActualizado;
				int posX,posY;
				bs >> nickPersonajeActualizado >> posX >> posY;
				//busco si es un enemigo
				Enemigo* unEnemigo = NULL;
				Golem* unGolem = NULL;
				int pos = 0;
				if(pm.enemyExists(nickPersonajeActualizado)){
					//busco el enemigo
					unEnemigo = pm.getEnemy(nickPersonajeActualizado);
				} 
				//busco si es un golem
				if(pm.golemExists(nickPersonajeActualizado)){
					//busco el golem
					unGolem = pm.getGolem(nickPersonajeActualizado);
				} 
	
				if(unEnemigo != NULL){

					int XSiguiente = unEnemigo->getXSiguiente();
					int YSiguiente = unEnemigo->getYSiguiente();
					if (XSiguiente == posX && YSiguiente == posY){
						//si es el primer cliente que me manda el update
						mapa.actualizarGrafo(unEnemigo->getX(),unEnemigo->getY());//actuzliazo grafo pos vieja
						unEnemigo->setPos(posX,posY);//actualizo posiciones		
						mapa.actualizarGrafoPersonajes(pm);//actuzliazo grafo nueva

						TileServidor* proxTile;
						string enemigoAtacado;//guarda el nick si va a atacar
						bool personajeAdyacente= unEnemigo->personaje_adyacente(mapa,pm,proxTile,enemigoAtacado); // me dice si hay un personaje en un tile adyacente para atacarlo
						if ( (personajeAdyacente && unEnemigo->get_timer_ataque().getTicks() >= tiempoAtaque) || (personajeAdyacente && !unEnemigo->get_ultima_accion_atacar()) ){
							unEnemigo->atacar(enemigoAtacado,pm,*this);
							unEnemigo->get_timer_ataque().start();
							unEnemigo->set_ultima_accion_atacar(true);
						}else {
							if(!personajeAdyacente){
								unEnemigo->set_ultima_accion_atacar(false);
								unEnemigo->get_timer_ataque().stop();
							}
							//si es un enemigo actualizo su posicion
							TileServidor* proxTile = unEnemigo->get_proximo_tile_enemigo(mapa,pm);
						
							if (proxTile != NULL && !mapa.tile_esta_ocupado(proxTile->get_x(),proxTile->get_y(),pm)){
								unEnemigo->setPosSiguiente(proxTile->get_x(),proxTile->get_y());
								// Informamos a los demas del movimiento del enemigo
								for(auto it = clients_map.begin();it != clients_map.end();it++) {
										bs.clear();
										bs << PROTO::MOVE_PLAYER << nickPersonajeActualizado << proxTile->get_x() << proxTile->get_y() ;
										it->second.send(bs.str());
										//std::cout << "Mandando update de enemigo a " << it->second.nick << "\n";
								}
							}
						}
					}
				}
					
				if(unGolem != NULL){

					int XSiguiente = unGolem->getXSiguiente();
					int YSiguiente = unGolem->getYSiguiente();
					if (XSiguiente == posX && YSiguiente == posY){
						//si es el primer cliente que me manda el update
						mapa.actualizarGrafo(unGolem->getX(),unGolem->getY());//actuzliazo grafo pos vieja
						unGolem->setPos(posX,posY);//actualizo posiciones		
						mapa.actualizarGrafoPersonajes(pm);//actuzliazo grafo nueva

						TileServidor* proxTile;
						string enemigoAtacado;//guarda el nick si va a atacar
						bool personajeAdyacente= unGolem->personaje_adyacente(mapa,pm,proxTile,enemigoAtacado); // me dice si hay un personaje en un tile adyacente para atacarlo
						if ( (personajeAdyacente && unGolem->get_timer_ataque().getTicks() >= tiempoAtaque) || (personajeAdyacente && !unGolem->get_ultima_accion_atacar()) ){
							unGolem->atacar(enemigoAtacado,pm,*this);
							unGolem->get_timer_ataque().start();
							unGolem->set_ultima_accion_atacar(true);
						}else {
							unGolem->set_ultima_accion_atacar(false);
							unGolem->get_timer_ataque().stop();
							//si es un enemigo actualizo su posicion
							TileServidor* proxTile = unGolem->get_proximo_tile_enemigo(mapa,pm);
						
							if (proxTile != NULL && !mapa.tile_esta_ocupado(proxTile->get_x(),proxTile->get_y(),pm)){
								unGolem->setPosSiguiente(proxTile->get_x(),proxTile->get_y());
								// Informamos a los demas del movimiento del enemigo
								for(auto it = clients_map.begin();it != clients_map.end();it++) {
										bs.clear();
										bs << PROTO::MOVE_PLAYER << nickPersonajeActualizado << proxTile->get_x() << proxTile->get_y() ;
										it->second.send(bs.str());
										std::cout << "Mandando update de enemigo a " << it->second.nick << "\n";
								}
							}
						}
					}
				}
			}else if(pt == PROTO::REQUEST_REV_POS) {
				int x, y;
				bs >> x >> y;
				bool ok = true;
				std::cout << "GOT REQUEST_REV_POS <" << new_nick << ">: " << x << ";" << y << "\n";
				//Veo si era el enemigo a matar
				bool terminoPartida=false;
				// Iteramos para ver si hay algun personaje
				for(auto it = pm.getPlayers().begin();it != pm.getPlayers().end();it++) {
					if (it->first == new_nick) continue;
					if(it->second.getX() == x && it->second.getY() == y) {
						ok = false;
						break;
					}
					if(it->second.getXSiguiente() == x && it->second.getYSiguiente() == y) {
						//si va a ahi
						ok = false;
						break;
					}
				}
				//para enemigos
				for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end();it++) {
					if(it->second->getX() == x && it->second->getY() == y) {
						//si esta ahi
						ok = false;
						break;
					}
					if(it->second->getXSiguiente() == x && it->second->getYSiguiente() == y) {
						//si va a ahi
						ok = false;
						break;
					}
				}
				// para golem
				for(auto it = pm.getGolems().begin();it != pm.getGolems().end();it++) {
					if(it->second->getX() == x && it->second->getY() == y) {
						//si esta ahi
						ok = false;
						break;
					}
					if(it->second->getXSiguiente() == x && it->second->getYSiguiente() == y) {
						//si va a ahi
						ok = false;
						break;
					}
				}
				if(ok) {
					// Si el movimiento esta ok, actualizamos la posicion
					pm.getPlayer(new_nick).setPos(x, y);
					// Lo descongelamos si estaba congelado y avisamos a todos
					pm.getPlayer(new_nick).descongelar();
					bs.clear();
					bs << PROTO::DESCONGELAR << new_nick;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						it->second.send(bs.str());
					}
					// Informamos al jugador que esta ok
					bs.clear();
					bs << PROTO::POS_REQUEST_REV_REPLY << true;
					std::cout << "REPLY_REV: OK\n";
					this->send(cid, bs.str());
					// Informamos a los demas del movimiento
					bs.clear();
					bs << PROTO::REV_PLAYER << new_nick << x << y;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						if(it->second.nick == new_nick) continue; // Salteamos el jugador en cuestion
						it->second.send(bs.str());
						std::cout << "Mandando update de revivir a " << it->second.nick << "\n";
					}
					//pm.getPlayer(new_nick).descongelar();
				}else{
					// Si fallo, avisamos al cliente
					bs.clear();
					bs << PROTO::POS_REQUEST_REV_REPLY << false;
					std::cout << "REPLY: FAIL\n";
					this->send(cid, bs.str());
				}
			}else if(pt == PROTO::CATCH_FLAG) {
				int x,y;
				bs >> x >> y;
				std::cout << new_nick << " atrapo bandera en pos (" << x << "," << y << ")" << endl;
				if (mision.hayBandera(x,y) && !pm.getPlayer(new_nick).tieneBandera(x,y)) {
					pm.getPlayer(new_nick).atrapoBandera(x,y);
					//Veo si gano la mision
					if (pm.getPlayer(new_nick).cantBanderas()==mision.cantBanderas()) {
						bs.clear();
						bs << PROTO::WINNER << new_nick;
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
							std::cout << "Mandando ganador de la mision a " << it->second.nick << "\n";
						}
						Sleep(6000);
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							for (auto itP = pm.getPlayers().begin(); itP != pm.getPlayers().end(); itP++) {
								bs.clear();
								bs << PROTO::RESET_PLAYER << itP->first << itP->second.getXInicial() << itP->second.getYInicial();
								it->second.send(bs.str());
								std::cout << "Mandando reset de " << itP->first << " de la mision a " << it->second.nick << "\n";
								itP->second.reiniciar();
							}
							 
						}
						for (auto itE = pm.getEnemies().begin(); itE != pm.getEnemies().end(); itE++) {
							delete (itE->second);
						}
						pm.getEnemies().clear();
						//Agregar enemigos automaticos
						pm.addEnemy("EnemigoMaster","soldado",mapa,1);
						//pm.addEnemy("Enemigo2","soldado",mapa,2);	
						//pm.addEnemy("Enemigo3","soldado",mapa,2);
						//Mato a todos los golems que quedaron
						for (auto itG = pm.getGolems().begin(); itG != pm.getGolems().end(); itG++) {
							delete (itG->second);
						}
						pm.getGolems().clear();	
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							for (auto itE = pm.getEnemies().begin(); itE != pm.getEnemies().end(); itE++) {							
								bs.clear();
								bs << PROTO::NEW_PLAYER << itE->second->getNick() << itE->second->getTipo() << itE->second->getX() << itE->second->getY() << itE->second-> isOn();
								it->second.send(bs.str());
								//Mando los atributos principales del enemigo
								bs.clear();
								bs << PROTO::INIT_ATT << itE->second->getNick() << (float)itE->second->getVelocidad() << itE->second->getEnergia() << itE->second->getMagia() << itE->second->getEnergiaEscudo() << itE->second->getTerremoto() << itE->second->getHielo() << (float)itE->second->getRadio();
								it->second.send(bs.str());
							}
						}
					}
				}				
			}else if (pt == PROTO::USO_GOLEM){
				string nickDuenio;
				int xDuenio,yDuenio;
				bs >> nickDuenio >> xDuenio >> yDuenio;
				string nickGolem = "Golem";
				stringstream indice;
				indice << pm.get_cant_golems_historicos();
				nickGolem = nickGolem + indice.str();
				pm.addGolem(nickGolem,"orco",mapa,1,nickDuenio,xDuenio,yDuenio);
				pm.sumar_un_golem();
				//mando el golem a todos
				Golem* unGolem = pm.getGolem(nickGolem);
				for(auto it = clients_map.begin();it != clients_map.end();it++) {
					bs.clear();
					bs << PROTO::NEW_PLAYER << unGolem->getNick() << unGolem->getTipo() << unGolem->getX() << unGolem->getY() << unGolem-> isOn();
					it->second.send(bs.str());
					//Mando los atributos principales del jugador
					bs.clear();
					bs << PROTO::INIT_ATT << unGolem->getNick() << (float)unGolem->getVelocidad() << unGolem->getEnergia() << unGolem->getMagia() << unGolem->getEnergiaEscudo() << unGolem->getTerremoto() << unGolem->getHielo() << (float)unGolem->getRadio();
					it->second.send(bs.str());
				}
			

			}else if (pt == PROTO::TRANSMUT){
				char tipo;
				bs >> tipo;
				std::string nick;
				bs >> nick;
				pm.getPlayer(new_nick).setTransmut(false);
				if (tipo == TIPO::LAPIDA) {
					// Aviso a todos que lo conviertan a lapida
					bs.clear();
					bs << PROTO::TRANSMUT << tipo << nick;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						it->second.send(bs.str());
					}
					// Lo congelo para que no pueda moverse
					if (pm.enemyExists(nick)) {
						pm.getEnemy(nick)->congelar();
					} else if (pm.golemExists(nick)) {
						pm.getGolem(nick)->congelar();
					} else {
						pm.getPlayer(nick).congelar();
					}
				} else if (tipo == TIPO::ESTRATEGIA_ENEMY) {
					bool bajaVel;
					bs >> bajaVel;
					if (pm.playerExists(nick)) {
						pm.getPlayer(nick).setVelocidadAnterior(pm.getPlayer(nick).getVelocidad());
						if(bajaVel){
							std::cout << "Enemigo " << nick << "bajando velocidad" << endl;
							pm.getPlayer(nick).setVelocidad(pm.getPlayer(nick).getVelocidad()*0.75);
						}else{
							std::cout << "Enemigo " << nick << "subiendo velocidad" << endl;
							pm.getPlayer(nick).setVelocidad(pm.getPlayer(nick).getVelocidad()*1.25);						
						}

						bs.clear();
						bs << PROTO::UPDATE_ATT << ATT::VEL << nick << (float)pm.getPlayer(nick).getVelocidad();
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
						}
					}
					if (pm.enemyExists(nick)) {
						pm.getEnemy(nick)->setVelocidadAnterior(pm.getEnemy(nick)->getVelocidad());
						if(bajaVel){
							std::cout << "Enemigo " << nick << "bajando velocidad" << endl;
							pm.getEnemy(nick)->setVelocidad(pm.getEnemy(nick)->getVelocidad()*0.75);
						}else{
							std::cout << "Enemigo " << nick << "subiendo velocidad" << endl;
							pm.getEnemy(nick)->setVelocidad(pm.getEnemy(nick)->getVelocidad()*1.25);						
						}

						bs.clear();
						bs << PROTO::UPDATE_ATT << ATT::VEL << nick << (float)pm.getEnemy(nick)->getVelocidad();
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
						}
					}
					if (pm.golemExists(nick)) {
						pm.getGolem(nick)->setVelocidadAnterior(pm.getGolem(nick)->getVelocidad());
						if(bajaVel){
							std::cout << "Enemigo " << nick << "bajando velocidad" << endl;
							pm.getGolem(nick)->setVelocidad(pm.getGolem(nick)->getVelocidad()*0.75);
						}else{
							std::cout << "Enemigo " << nick << "subiendo velocidad" << endl;
							pm.getGolem(nick)->setVelocidad(pm.getGolem(nick)->getVelocidad()*1.25);						
						}

						bs.clear();
						bs << PROTO::UPDATE_ATT << ATT::VEL << nick << (float)pm.getGolem(nick)->getVelocidad();
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
						}
					}
				}
			}else if (pt == PROTO::DESTRANSMUT){
				char tipo;
				bs >> tipo;
				std::string nick;
				bs >> nick;
				if (tipo == TIPO::LAPIDA) {
					// Aviso a todos que lo conviertan a lapida
					bs.clear();
					bs << PROTO::DESTRANSMUT << tipo << nick;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						it->second.send(bs.str());
					}
					// Lo descongelo para que pueda volver a moverse
					if (pm.enemyExists(nick)) {
						pm.getEnemy(nick)->descongelar();
					} else if (pm.golemExists(nick)) {
						pm.getGolem(nick)->descongelar();
					} else {
						pm.getPlayer(nick).descongelar();
					}
				} else if (tipo == TIPO::ESTRATEGIA_ENEMY) {
					if (pm.playerExists(nick)) {
						pm.getPlayer(nick).setVelocidad(pm.getPlayer(nick).getVelocidadAnterior());
						std::cout << "Enemigo " << nick << "destrsansmut velocidad" << endl;

						bs.clear();
						bs << PROTO::UPDATE_ATT << ATT::VEL << nick << (float)pm.getPlayer(nick).getVelocidad();
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
						}
					}
					if (pm.enemyExists(nick)) {
						pm.getEnemy(nick)->setVelocidad(pm.getEnemy(nick)->getVelocidadAnterior());
						std::cout << "Enemigo " << nick << "destrsansmut velocidad" << endl;

						bs.clear();
						bs << PROTO::UPDATE_ATT << ATT::VEL << nick << (float)pm.getEnemy(nick)->getVelocidad();
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
						}
					}
					if (pm.golemExists(nick)) {
						pm.getGolem(nick)->setVelocidad(pm.getGolem(nick)->getVelocidadAnterior());
						std::cout << "Enemigo " << nick << "destrsansmut velocidad" << endl;

						bs.clear();
						bs << PROTO::UPDATE_ATT << ATT::VEL << nick << (float)pm.getGolem(nick)->getVelocidad();
						for(auto it = clients_map.begin();it != clients_map.end();it++) {
							it->second.send(bs.str());
						}
					}
				}
			}else if (pt == PROTO::DEAD){
				std::string nick_who;
				bs >> nick_who;
				cout << nick_who << " fue asesinado" << endl;
				if (pm.playerExists(nick_who)) {
					pm.getPlayer(nick_who).congelar();
				} else if (pm.enemyExists(nick_who)) {
					// Lo borro -->se borra en el damage
					/*for (auto it = pm.getEnemies().begin(); it != pm.getEnemies().end(); it++) {
						if (it->first == nick_who) pm.getEnemies().erase(it);
					}*/
					// Aviso a todos que esta muerto
					bs.clear();
					bs << PROTO::ENEMY_DEAD << nick_who;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						it->second.send(bs.str());
					}

				}else if(pm.golemExists(nick_who)){
					bs.clear();
					bs << PROTO::ENEMY_DEAD << nick_who;
					for(auto it = clients_map.begin();it != clients_map.end();it++) {
						it->second.send(bs.str());
					}
				}
			}else{
				assert(false);
				bs.clear();
				bs << PROTO::TEXTMSG << std::string("Unknown packet type");
				this->send(cid, bs.str());
			}
			LeaveCriticalSection(&critSect);
		}

		// Cuando hay una desconexion loggeamos e informamos al resto
		bs.clear();
		bs << PROTO::PLAYER_EXIT << new_nick;
		removeClient(cid);
		pm.getPlayer(new_nick).setOffline();
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
std::map<std::string, Client>& ServerSocket::get_clients(){
	return clients_map;
}