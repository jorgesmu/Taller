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
#include "enemigoServer.h"
#include "../source/net/bitstream.h"
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
bool puedeMoverseEnemigo = false;
bool conectandose=false;
bool crearMision = true;
//dependencias externas
extern BitStream bs;
unsigned int enemyCount = 0;
int main(int argc, char* argv[]) {

	// Parseamos el escenario a elegir
	if(argc != 2) {
		//std::cerr << "Falta especificar el escenario:\n \tservidor.exe <escenario>\n";
		escenario_elegido = "a";
		crearMision = false;
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
	
	//Agregar enemigos automaticos
	pm.addEnemy("Enemigo1","soldado",mapa,1);
	pm.addEnemy("Enemigo2","soldado",mapa,2);	
	pm.addEnemy("Enemigo3","soldado",mapa,2);
	if(crearMision){
		//Creacion de misiones(TODO:logica random entre los distintos tipos)
		mision.crearMisionBanderas(2);
		//std::cout << "Se creo mision de " << mision.cantBanderas() << " banderas" << endl;
		//mision.crearMisionEnemigo("derecha");
	}

	//Enemigo* unEnemigo = pm.getEnemy("Enemigo1");

	mapa.cargarGrafo(pm);
	mapa.actualizarGrafoPersonajes(pm);
	// Spawneamos el thread de listen
	_beginthreadex(NULL, 0, ServerSocket::listenLoopEntry, (void*)&sock, 0, NULL);
	bool mando = false;
	while(true) {
/*
		if(!conectandose){
	
			puedeMoverseEnemigo= true; 
			// Informamos a los demas del movimiento del enemigo
			std::map<std::string, Client> clients_map = sock.get_clients();
			for(auto it = clients_map.begin();it != clients_map.end();it++) {
				if(!mando){
					Sleep(8000);
					bs.clear();
					std::string nick = "Enemigo1";
					bs << PROTO::MOVE_PLAYER << nick << 2 <<2 ;

					sock.send(it->second.sock, bs.str());
					std::cout << "Mandando update a " << it->second.nick << "\n";
					mando = true;
				}
			}
		}*/
		Sleep(100);
	}
	//destruyo enemigos y golem 
	for(auto it = pm.getEnemies().begin();it != pm.getEnemies().end();it++) {
		delete(it->second);
	}

	// Close
	sock.close();
	return 0;
}