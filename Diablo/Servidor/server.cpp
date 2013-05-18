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
#include "../source/utilities/aux_func.h"
#pragma comment(lib, "Ws2_32.lib")
#include "config_servidor.h"
//#include "../source/utilities/config_juego.h"
#include "parserServer.h"
#include "playerman.h"
#include "mapaservidor.h"
#include <cstdlib>
#include <ctime>

logErrores err_log("log_parser.txt");
PlayerManager pm;
MapaServidor mapa;

config_juego juego;
config_pantalla* pantalla;
vector <config_entidad> entidades;
config_general configuracion;
vector <config_escenario> escenarios;

int main(int argc, char* args[]) {
	
	// Iniciamos el seed de srand
	std::srand(std::time(NULL));

	ServerSocket sock;
	// Init
	if(!sock.init())
		return 1;
	config_servidor config = parsear_red_server("../servidor/resources_server/red.yaml");
	// Bind
	if(!sock.listen(config.get_puerto()))
		return 1;

	// Parsea la configuracion del mapa y entidades
	juego = parsear_juego("../servidor/resources2client/niveles.yaml");
	pantalla = juego.get_pantalla();
	entidades = juego.get_entidades();
	configuracion = juego.get_configuracion();
	escenarios = juego.get_escenarios();
	
	// Cargamos el mapa
	// TODO: leer por consola y validar que exista
	mapa.resize(escenarios[0].get_tam_x(), escenarios[0].get_tam_x());



	// Spawneamos el thread de listen
	_beginthreadex(NULL, 0, ServerSocket::listenLoopEntry, (void*)&sock, 0, NULL);

	while(true) {
		Sleep(100);
	}

	// Close
	sock.close();
	return 0;
}