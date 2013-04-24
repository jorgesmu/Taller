#include "serversocket.h"
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

	// Aceptar 1 conexion
	std::string cid;
	if(!sock.accept(cid))
		return 1;

	// Mandar algo
	sock.send(cid, "hola");

	// Close
	sock.close();

	return 0;
}