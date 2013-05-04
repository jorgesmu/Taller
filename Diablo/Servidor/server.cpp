#include "SDL.h"

#include "serversocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

#include "../source/net/bitstream.h"
#include "../source/net/defines.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* args[]) {
	
	ServerSocket sock;
	// Init
	if(!sock.init())
		return 1;

	// Bind
	if(!sock.listen(8080))
		return 1;

	// Aceptar conexiones indefinidamente
	while(sock.accept()) {
		//
	}

	// Close
	sock.close();
	return 0;
}