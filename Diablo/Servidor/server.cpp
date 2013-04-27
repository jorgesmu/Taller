#include "serversocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

int main() {
	
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