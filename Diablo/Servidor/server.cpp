#include "SDL.h"

#include "serversocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

#include "../source/net/bitstream.h"
#include "../source/net/defines.h"
#include "parserServer.h"
#include "logErroresServer.h"
#pragma comment(lib, "Ws2_32.lib")
#include "config_servidor.h"

logErrores err_log("log_parser.txt");
int main(int argc, char* args[]) {
	
	ServerSocket sock;
	// Init
	if(!sock.init())
		return 1;
	config_servidor config = parsear_red("red.yaml");
	// Bind
	if(!sock.listen(config.get_puerto()))
		return 1;

	// Aceptar conexiones indefinidamente
	while(sock.accept()) {
		//
	}

	// Close
	sock.close();
	return 0;
}