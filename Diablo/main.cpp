#include "source/net/bitstream.h"
#include "source/net/defines.h"
#include "Cliente/clientsocket.h"
#pragma comment(lib, "Ws2_32.lib")
#include <Windows.h>
#include <process.h>
#include <iostream>
#include "source/utilities/logErrores.h"
// SDL Headers
#include "SDL.h"
#include <SDL_getenv.h>
// Our headers
#include "../../source/utilities/timer.h"
#include "../../source/utilities/surface.h"
#include "../../source/utilities/aux_func.h"
#include "../../source/utilities/parser.h"
#include "../../source/display/entidad.h"
#include "../../source/utilities/Personaje.h"
#include "../../source/display/entidadFija.h"
#include "../../source/display/camara.h"
#include "../../source/display/mapa.h"
#include "../../source/display/resman.h"
#include "../../source/constants/model.h"
#include "../../source/utilities/Test.h"
#include "../../source/utilities/coordenadas.h"
#include "../../source/utilities/config_cliente.h"
#include "../../source/net/PjeManager.h"
#include "../../source/utilities/chatwindow.h"

using namespace std;

logErrores err_log("log_cliente.txt");
bool pasoArchivos = false;

// Variables globales
// Critical section
CRITICAL_SECTION cs_main;
// Mapa
Mapa mapa;
// Personaje Manager
PjeManager pjm;
// Configuracion del personaje local
std::string pje_local_nick;
std::string pje_local_tipo;
int start_pos_x, start_pos_y;
int escenario_elegido_id;
// Cosas para mantener al server actualizado sobre los tiles que recorrimos
struct {
	vec2<int> tile_actual, tile_anterior;
	Timer timer;
	static const int INTERVAL = 200; // ms
} update_recorrido;
// Ventana de chat
ChatWindow chat_window;

int main(int argc, char* argv[]) {
	// Verificamos que se pase el nick y el tipo
/*	if(argc != 3) {
		std::cout << "Falta especificar nick:\ncliente.exe <nick> <tipo_personaje>\n";
		return 0;
	}else{
		// Cargamos el nick y tipo de la consola
		pje_local_nick = argv[1];
		pje_local_tipo = argv[2];
	}
*/
	//borrar estas dos lineas
	pje_local_nick = "jugador";
	pje_local_tipo = "soldado";
	escenario_elegido_id = 0;

	InitializeCriticalSection(&cs_main);
	// Socket de cliente
	ClientSocket sock(&cs_main);
	if(!sock.init()) 
		return 1;

	//cargo ip servidor y puerto
	config_cliente configuracion_red = parsear_red("../resources/static/red.yaml");
	// Conectamos
	if(!sock.connect(configuracion_red.get_ip_servidor(),configuracion_red.get_puerto()))
		return 1;
	
	// Creamos el thread de listen
	HANDLE hth1 = (HANDLE)_beginthreadex(NULL, 0, ClientSocket::listenEntry, (void*)&sock, 0, NULL);
	// Mandamos el nick
	BitStream bs;
	//bs << PROTO::NICK << std::string(argv[1]);
	bs << PROTO::NICKANDTYPE << pje_local_nick << pje_local_tipo;
	sock.send(bs.str());

	// Esperamos el paso de archivos
	while (!pasoArchivos){
		Sleep(50);
	}
	//Empieza a dibujar
	// Parseo el nivel
	config_juego juego = parsear_juego("../resources/niveles.yaml");
	config_pantalla* pantalla = juego.get_pantalla();
	vector <config_entidad> entidades = juego.get_entidades();
	config_general configuracion = juego.get_configuracion();
	vector <config_escenario> escenarios = juego.get_escenarios();
			
	// Ventana de prueba
	SDL_Surface* screen;
	putenv("SDL_VIDEO_CENTERED=1"); // Para centrar la ventana
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) { std::cerr << "Error @ SDL_Init(): " << SDL_GetError() << "\n"; return -1; }
	// Init the window
	screen = SDL_SetVideoMode(pantalla->get_ancho(), pantalla->get_alto(), 32, SDL_SWSURFACE|SDL_NOFRAME);
	// Init a SDL_TTF
	if(TTF_Init() == -1) { std::cerr << "Error @ TTF_Init(): " << TTF_GetError() << "\n"; return -1; }
	// Para confinar el mouse a la ventana
	SDL_WM_GrabInput(SDL_GRAB_ON);
	// Lo movemos al medio
	SDL_WarpMouse(pantalla->get_ancho()/2, pantalla->get_alto()/2);
	SDL_WM_SetCaption("Diablo", NULL);

	// Camara
	Camara camara;
	camara.init(pantalla->get_ancho(), pantalla->get_alto(), configuracion.get_margen_scroll());

	mapa.resize(escenarios[escenario_elegido_id].get_tam_x(), escenarios[escenario_elegido_id].get_tam_x());
	// Resman
	ResMan resman;
	if(!resman.init()) return -2;
	
	// Ventana de chat
	chat_window.init(&resman, 40, 40, Font::SIZE_NORMAL, 250, 500, COLOR::WHITE);
	chat_window.setNickLocal(pje_local_nick);

	// Cargo la entidad por default
	resman.addRes("tierraDefault", "../resources/tile.png");
	Entidad entidadPisoPorDefecto("tierraDefault", 1 , 1 , true , 0 , 0 , NULL, resman , Imagen::COLOR_KEY);

	// Cargamos el tile por defecto
	for(auto it = mapa.allTiles().begin();it != mapa.allTiles().end(); ++it) {
		if(it->sinEntidades()) {
			it->addEntidad((Entidad*)&entidadPisoPorDefecto);
		}
	}

	// Cargo las entidades en un vector
	std::vector<EntidadFija*> entidades_cargadas;
	for (auto it = entidades.begin(); it != entidades.end(); ++it){
		resman.addRes(it->get_nombre(), it->get_path_imagen(), Imagen::COLOR_KEY);
		EntidadFija* entidad;
		if(it->get_fps() > 0 && it->get_alto_sprite() > 0 && it->get_ancho_sprite() > 0){
			entidad = new EntidadFija (it->get_nombre(), it->get_ancho_base(), it->get_alto_base(), it->get_caminable(), it->get_fps(), it->get_delay(),
								it->get_alto_sprite(), it->get_ancho_sprite(), it->get_pixel_ref_x(), it->get_pixel_ref_y(), NULL, &mapa, resman, Imagen::COLOR_KEY);
		}else{
			entidad = new EntidadFija (it->get_nombre(), it->get_ancho_base(), it->get_alto_base(), it->get_caminable(), it->get_pixel_ref_x(), it->get_pixel_ref_y(),
								NULL, &mapa, resman, Imagen::COLOR_KEY);
		}
		entidades_cargadas.push_back(entidad);
	}

	// Vector de entidades en este mapa
	vector<config_entidad_en_juego> entidades_en_juego = escenarios[escenario_elegido_id].get_entidades();

	// Llenamos el mapa con las entidades
	for(auto it = entidades_en_juego.begin(); it != entidades_en_juego.end(); ++it){
		bool entidad_encontrada = false;
		for(auto it2 = entidades_cargadas.begin(); it2 != entidades_cargadas.end(); ++it2){
			if(it->get_nombre() == (*it2)->get_nombre()){
				EntidadFija* copia;
				
				copia = new EntidadFija (*it2, &mapa);

				mapa.getTile(it->get_pos_x(), it->get_pos_y())->addEntidad(copia);
				(copia)->setTileActual(mapa.getTile(it->get_pos_x(), it->get_pos_y()), &mapa);
				entidad_encontrada = true;
				break;
			}
		}
		if(!entidad_encontrada) {
			std::stringstream ss;
			ss << "Entidad " << it->get_nombre() << " no fue cargada porque no fue definida\n";
			err_log.escribir(ss.str());
		}
	}
	 


	// Agrega el personaje
	pjm.getPjeLocal().init(pje_local_tipo , 1 , 1 , 50 , 5, 100, 100 ,	configuracion.get_vel_personaje(),	0 , 70 , NULL , resman , Imagen::COLOR_KEY);
	// Posiciono el personaje
	start_pos_x = 0;
	start_pos_y = 0;
	mapa.getTile(start_pos_x, start_pos_y)->addEntidad(&(pjm.getPjeLocal()));
	pjm.getPjeLocal().setTileActual(mapa.getTile(start_pos_x, start_pos_y));
	// Inicializo el recorridor
	update_recorrido.tile_anterior = update_recorrido.tile_actual = vec2<int>(start_pos_x, start_pos_y);
	update_recorrido.timer.start();
	// Centro la camara
	camara.center(mapa.getTile(start_pos_x, start_pos_y)->getX(), mapa.getTile(start_pos_x, start_pos_y)->getY());

	// Variables para el game-loop
	double curr_time = SDL_GetTicks();
    double accum = 0.0;
	bool quit = false;

	// Para guardar los eventos  de input
	SDL_Event event;


	Tile::setearExplorados(20,20, &pjm.getPjeLocal(), &mapa);


	while((!quit ) && (sock.isOpen()) ) {

		// Sync stuff
		EnterCriticalSection(&cs_main);

		// Input handling (esto despues se movera a donde corresponda)
		while(SDL_PollEvent(&event)) {

			if(chat_window.isOpen()) {

				int res = chat_window.handleInput(event);
				if(res == SInput::RES_CLOSE) {
					chat_window.hide();
				}else if(res == SInput::RES_ENTER) {
					// Enviar el mensaje al servidor
				}

			}else{

				// Detectar escape o quit
				if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) {
					quit = true;
				}
				if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
					chat_window.open();
				}
				// Detectar mouse motion
				if(event.type == SDL_MOUSEMOTION) {
					// Update para la camara
					camara.update_speed(makeRect(event.motion.x, event.motion.y));
				}
				// Mouse clicks
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						vec2<int> tile_res = MouseCoords2Tile(vec2<int>(event.button.x, event.button.y), camara);
						if(mapa.tileExists(tile_res.x, tile_res.y)) {
							pjm.getPjeLocal().mover(mapa.getTile(tile_res.x,tile_res.y));							

							//Tile* tilePersonaje = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
							//Tile* tileDestino = mapa.getTile(tile_res.x, tile_res.y);
							//tileDestino = mapa.getTile(tile_res.x, tile_res.y);
							//vector<pair<int, int> > caminoMinimo = mapa.getCaminoMinimo(tilePersonaje, tileDestino, pjm);
							//int indice = 1;
							//while(pjm.getPjeLocal().getX() != tileDestino->getX() && pjm.getPjeLocal().getY() != tileDestino->getY()){						
							//	pair<int, int> proximoTile = caminoMinimo[indice];
							//	pjm.getPjeLocal().mover(mapa.getTile(proximoTile.first, proximoTile.second));
							//	Tile* tileActual = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
							//	while (tileActual->getU() != proximoTile.first || tileActual->getV() != proximoTile.second){
							//		// Actualizamos el personaje principal
							//		pjm.getPjeLocal().update(&mapa);
							//		tileActual = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
							//	}
							//	indice ++;
							//}
						}
					}
				}

			}
		}

		// Cuenterio para hacer el timestep (CONST_DT) independiente de los FPS
		// (ver http://gafferongames.com/game-physics/fix-your-timestep/)
		double new_t = SDL_GetTicks();
		double frame_time = new_t - curr_time;
		if(frame_time > 250) {
			// note: max frame time to avoid spiral of death
			frame_time = 250; 
		}
		curr_time = new_t;
		accum += frame_time;

		// Aca se hace el timestep, aka avanzar la fisica usando Euler en un delta t fijo
		while(accum >= CONST_DT) {
			// Actualiza la camara
			camara.update();
			// Actualiza las entidades
			for(auto it = entidades_cargadas.begin(); it != entidades_cargadas.end(); ++it){
				(*it)->update(&mapa);
			}
			// Actualizamos los personajes
			for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end(); it++) {
				it->second.update(&mapa);
			}
			// Actualizamos el personaje principal
			pjm.getPjeLocal().update(&mapa);
			
			// Update a tiles recorridos
			if(update_recorrido.timer.getTicks() > update_recorrido.INTERVAL) {
				update_recorrido.timer.start();
				auto t = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
				update_recorrido.tile_actual = vec2<int>(t->getU(), t->getV());
				//std::cout << "Tile actual: (" << update_recorrido.tile_actual.x << "," << update_recorrido.tile_actual.y << ")\n";
				if(update_recorrido.tile_actual != update_recorrido.tile_anterior) {
					update_recorrido.tile_anterior = update_recorrido.tile_actual;
					BitStream bs;
					bs << PROTO::NIEBLA_SYNC << update_recorrido.tile_actual.x << update_recorrido.tile_actual.y;
					sock.send(bs.str());
				}
			}

			// Decrease al accum
			accum -= CONST_DT;
		}

		// Aca se hace el blitteo
		// Despues se movera a donde corresponda
		SDL_FillRect(screen, NULL, 0);
		// Draw el mapa
		mapa.blit(screen, camara, &(pjm.getPjeLocal()));
		mapa.setEntidadesDibujadasFalse();
		// Dibujamos la ventana de chat
		chat_window.show(screen);
		// Actualizar la pantalla
		SDL_Flip(screen);

		LeaveCriticalSection(&cs_main);

	}
	
	mapa.clean();
	resman.clean();
	err_log.cerrarConexion();
    //Test::test();
	TTF_Quit();
	SDL_Quit();
	if(!sock.isOpen()){
		cout << "Conexion cerrada por el servidor" << endl;
		system ("PAUSE");
	}
	sock.close();
	return 0;
}