#include "SDL.h"

#include "serversocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>
#include <cassert>

#include "../source/net/bitstream.h"
#include "../source/net/defines.h"
#include "parserServer.h"
#include "logErroresServer.h"
#include "../source/utilities/aux_func.h"
#pragma comment(lib, "Ws2_32.lib")
#include "config_servidor.h"
#include "misiones.h"
//#include "../source/utilities/config_juego.h"
#include "parserServer.h"
#include "playerman.h"
#include "mapaservidor.h"
#include <cstdlib>
#include <ctime>

logErrores err_log("log_parser.txt");
PlayerManager pm;
MapaServidor mapa;
Misiones mision;
std::string escenario_elegido;
int escenario_elegido_id;

config_juego juego;
config_pantalla* pantalla;
vector <config_entidad> entidades;
config_general configuracion;
vector <config_escenario> escenarios;

int main(int argc, char* argv[]) {

	// Parseamos el escenario a elegir
	if(argc != 2) {
		std::cerr << "Falta especificar el escenario:\n \tservidor.exe <escenario>\n";
		return -1;
	}else{
		escenario_elegido = argv[1];
	}

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
	
	// Cargamos el mapa y el escenario
	// Si el escenario es invalido, usamos el escenario 0
	escenario_elegido_id = 0;
	int x = 0;
	for(auto it = escenarios.begin();it != escenarios.end();it++) {
		if(it->get_nombre() == escenario_elegido) {
			escenario_elegido_id = x;
			std::cout << "Se encontro el escenario (" << escenario_elegido_id << ")\n";
			break;
		}else{
			x++;
		}
	}
	// Verificamos si encontramos el escenario
	if(x == escenarios.size()) {
		std::cerr << "Escenario <" << escenario_elegido << "> no encontrado, defaulteando al primer escenario\n";
	}
	// Cargamos el escenario en el mapa
	mapa.resize(escenarios[escenario_elegido_id].get_tam_x(), escenarios[escenario_elegido_id].get_tam_x());
	// Cargamos los tiles no caminables
	auto entidades_en_escenario = escenarios[escenario_elegido_id].get_entidades();
	for(auto it = entidades_en_escenario.begin();it != entidades_en_escenario.end();it++) {
		std::string ent = it->get_nombre();
		int ent_x = it->get_pos_x();
		int ent_y = it->get_pos_y();
		int ent_ancho = 1;
		int ent_alto = 1;
		// Buscamos la entidad
		bool found = false;
		bool es_caminable = true;
		for(auto it_def = entidades.begin(); it_def != entidades.end();it_def++) {
			if(ent == it_def->get_nombre()) {
				ent_ancho = it_def->get_ancho_base();
				ent_alto = it_def->get_alto_base();
				es_caminable = it_def->get_caminable();
				found = true;
				break;
			}
		}
		if(!found) {
			// Si no la encontramos, la ignoramos
		}else{
			// Si la encontramos y no es caminable, actualizamos en el mapa
			if(!es_caminable) {
				for(int x = ent_x;x < ent_x+ent_ancho;x++) {
					for(int y = ent_y;y < ent_y+ent_alto;y++) {
						assert(mapa.tileExists(x, y));
						mapa.getTile(x, y)->setNoCaminable();
						//std::cout << "MARCAMOS EL TILE (" << x << "," << y << ") NO CAMINABLE\n";
					}
				}
			}
		}
	}
	//Creacion de misiones()logica random entre los distintos tipos)
	mision.crearMisionBanderas(2);
	std::cout << "Se creo mision de " << mision.cantBanderas() << " banderas" << endl;

	// Spawneamos el thread de listen
	_beginthreadex(NULL, 0, ServerSocket::listenLoopEntry, (void*)&sock, 0, NULL);

	while(true) {
		Sleep(100);
	}

	// Close
	sock.close();
	return 0;
}