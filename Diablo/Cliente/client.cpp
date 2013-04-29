#include "clientsocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
	
using namespace std;

int main() {

	ClientSocket sock;
	
	if(!sock.init()) 
		return 1;
	
	if(!sock.connect("127.0.0.1", 8080))
		return 1;
	
	// Creamos el thread de listen
	HANDLE hth1 = (HANDLE)_beginthreadex(NULL, 0, ClientSocket::listenEntry, (void*)&sock, 0, NULL);
	// Hacemos el input en el main thread
	std::string line;
	while(std::getline(std::cin, line)) {
		if(!sock.isOpen() || !sock.send(line)) 
			break;
	}

	sock.close();
	return 0;
}