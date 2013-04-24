#include "clientsocket.h"
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
	
	//if(!sock.send("hola\n")) return 1;

	std::string stuff;
	while(sock.receive(stuff)) {
		std::cout << stuff << "\n";
	}

	sock.close();
	return 0;
}