#include "SDL.h"

#include "clientsocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

#include "../source/net/bitstream.h"
#include "../source/net/defines.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {

	// Verificamos que se pase el nick
	if(argc == 1) {
		std::cout << "Falta especificar nick:\ncliente.exe <nick>\n";
		return 0;
	}

	ClientSocket sock;
	
	if(!sock.init()) 
		return 1;
	
	if(!sock.connect("127.0.0.1", 8080))
		return 1;
	
	// Creamos el thread de listen
	HANDLE hth1 = (HANDLE)_beginthreadex(NULL, 0, ClientSocket::listenEntry, (void*)&sock, 0, NULL);
	// Mandamos el nick
	BitStream bs;
	bs << PROTO::NICK << std::string(argv[1]);
	sock.send(bs.str());
	// Hacemos el input en el main thread
	std::string line;
	while(sock.isOpen() && std::getline(std::cin, line)) {
		if(!sock.send(line)) 
			break;
	}

	sock.close();
	return 0;
}