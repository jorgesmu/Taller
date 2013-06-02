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
#include "../../source/display/entidadFijaCaminable.h"
#include "../../source/display/camara.h"
#include "../../source/display/mapa.h"
#include "../../source/display/resman.h"
#include "../../source/constants/model.h"
#include "../../source/utilities/Test.h"
#include "../../source/utilities/coordenadas.h"
#include "../../source/utilities/config_cliente.h"
#include "../../source/net/PjeManager.h"
#include "../../source/utilities/chatwindow.h"
#include "../../source/utilities/item.h"
#include "../../source/utilities/lampara.h"
#include "../../source/utilities/mapaItem.h"
#include "../../source/utilities/zapatos.h"
#include "../../source/utilities/terremoto.h"
#include "../../source/utilities/escudo.h"

using namespace std;

logErrores err_log("log_cliente.txt");
bool pasoArchivos = false;
bool cargoMapa = false;
const int NOSEMOVIO = -1;
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
double init_vel;
float init_radio;
char init_energia,init_magia,init_escudo,init_terremoto,init_hielo;
// Cosas para mantener al server actualizado sobre los tiles que recorrimos
struct {
	vec2<int> tile_actual, tile_anterior;
	Timer timer;
	static const int INTERVAL = 200; // ms
} update_recorrido;
// Cosas de movimiento del personaje
pair <int,int> tileActual, proximoTile;
int estadoMovimiento;
// Ventana de chat
ChatWindow chat_window;
// ResMan
ResMan resman;
// Config
config_general configuracion;
//Choques
bool choco;
//Socket
ClientSocket sock;

int main(int argc, char* argv[]) {
	// Verificamos que se pase el nick y el tipo
	//if(argc != 3) {
	//	std::cout << "Falta especificar nick:\ncliente.exe <nick> <tipo_personaje>\n";
	//	return 0;
	//}else{
	//	// Cargamos el nick y tipo de la consola
	//	pje_local_nick = argv[1];
	//	pje_local_tipo = argv[2];
	//}
	//borrar estas dos lineas
	pje_local_nick = "jugador";
	pje_local_tipo = "soldado";
	//escenario_elegido_id = 0;

	InitializeCriticalSection(&cs_main);
	// Socket de cliente
	if(!sock.init(&cs_main)) 
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
	configuracion = juego.get_configuracion();
	vector <config_escenario> escenarios = juego.get_escenarios();
			
	// Ventana de prueba
	SDL_Surface* screen;
	putenv("SDL_VIDEO_CENTERED=1"); // Para centrar la ventana
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) { std::cerr << "Error @ SDL_Init(): " << SDL_GetError() << "\n"; return -1; }
	// Init the window 
	screen = SDL_SetVideoMode(pantalla->get_ancho(), pantalla->get_alto(), 32, SDL_SWSURFACE);
	// Init a SDL_TTF
	if(TTF_Init() == -1) { std::cerr << "Error @ TTF_Init(): " << TTF_GetError() << "\n"; return -1; }
	// Para confinar el mouse a la ventana
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	// Lo movemos al medio
	SDL_WarpMouse(pantalla->get_ancho()/2, pantalla->get_alto()/2);
	SDL_WM_SetCaption("Diablo", NULL);

	mapa.resize(escenarios[escenario_elegido_id].get_tam_x(), escenarios[escenario_elegido_id].get_tam_x());

	// Camara
	Camara camara;
	camara.init(pantalla->get_ancho(), pantalla->get_alto(), configuracion.get_margen_scroll(), mapa);

	// Resman
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
			
	//Prueba de carga items
	resman.addRes("cofre","../resources/chest.png");
	MapaItem cofre("cofre",1,1,true, 6 ,13,NULL,&mapa,resman,Imagen::COLOR_KEY );
	mapa.getTile(6,13)->addEntidad(&cofre,&mapa);
	entidades_cargadas.push_back(&cofre);

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


	 
	// Avisamos al otro thread que ya cargo el mapa
	cargoMapa = true;

	// Agrega el personaje(no esta tomando velocidad del YAML!!!)
	pjm.getPjeLocal().init(pje_local_nick, pje_local_tipo , 1 , 1 , 50 , 5, 100, 100 ,	configuracion.get_vel_personaje(),	0 , 50 , NULL , resman , Imagen::COLOR_KEY);
	//Si no me conecto por primera vez al servidor
	if (init_vel!=0) {
		// El server me paso atributos para cargar
		pjm.getPjeLocal().setVelocidad(init_vel);
		pjm.getPjeLocal().setEnergia(init_energia);
		pjm.getPjeLocal().setMagia(init_magia);
		pjm.getPjeLocal().setEnergiaEscudo(init_escudo);
		pjm.getPjeLocal().setTerremoto(init_terremoto);
		pjm.getPjeLocal().setHielo(init_hielo);
		pjm.getPjeLocal().setRadio(init_radio);
	} else {
		//Aviso al server mis valores por defecto de atributos
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::VEL << (float)pjm.getPjeLocal().getVelocidad();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::ENERGIA << pjm.getPjeLocal().getEnergia();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::MAGIA << pjm.getPjeLocal().getEnergia();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::ENERGIA_ESCUDO << pjm.getPjeLocal().getEnergiaEscudo();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::CANT_TERREMOTO << pjm.getPjeLocal().getTerremoto();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::CANT_HIELO << pjm.getPjeLocal().getHielo();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::RADIO << pjm.getPjeLocal().getRadioY();
		sock.send(bs.str());
	}
	
	// Posiciono el personaje
	mapa.getTile(start_pos_x, start_pos_y)->addEntidad(&(pjm.getPjeLocal()));
	pjm.getPjeLocal().setTileActual(mapa.getTile(start_pos_x, start_pos_y));
	// Inicializo el recorridor
	update_recorrido.tile_anterior = update_recorrido.tile_actual = vec2<int>(start_pos_x, start_pos_y);
	update_recorrido.timer.start();
	// Cosas del movimiento
	estadoMovimiento = MOV::IDLE;
	// Centro la camara
	camara.center(mapa.getTile(start_pos_x, start_pos_y)->getX(), mapa.getTile(start_pos_x, start_pos_y)->getY());

	// Variables para el game-loop
	double curr_time = SDL_GetTicks();
    double accum = 0.0;
	bool quit = false;

	// Para guardar los eventos  de input
	SDL_Event event;

	//variables para el control del movimiento
	vector< pair<int,int> > caminoMinimo;
	int indice = 0; //indica que paso del movimiento se encuentra
	int estadoPersonaje = 0; //estado del personaje
	bool puedeMoverse = false; // indica si el personaje ya termino un movimiento anterior y puede seguir su camino o esta esperando un nuevo camino 
	mapa.cargarGrafo(); //cargo el grafo con la configuracion inicial
	int ultimoMovimientoX = NOSEMOVIO; // ultimas posiciones pasadas al mover
	int ultimoMovimientoY = NOSEMOVIO;// idem coordenada y
	int ultimoDestinoX = NOSEMOVIO;//guarda el destino actual
	int ultimoDestinoY = NOSEMOVIO;//guarda el destino actual
	
	while((!quit ) && (sock.isOpen()) ) {

		// Sync stuff
		EnterCriticalSection(&cs_main);

		// Input handling (esto despues se movera a donde corresponda)
		while(SDL_PollEvent(&event)) {

			if(chat_window.isOpen()) {
				chat_window.handleInput(event, sock);

			}else{

				// Detectar escape o quit
				if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) {
					quit = true;
				}
				// Detectar mouse motion
				if(event.type == SDL_MOUSEMOTION) {
					// Update para la camara
					camara.update_speed(makeRect(event.motion.x, event.motion.y));
				}
				// Atacar Defender Muerte
				if(event.type == SDL_KEYDOWN) {
					
					switch (event.key.keysym.sym) {
						case 'a' : {
							int posX = -1;	
							int posY = -1;
							SDL_GetMouseState(&posX, &posY);
							vec2<int> tile_res = MouseCoords2Tile(vec2<int>(posX,  posY), camara);
							if(mapa.tileExists(tile_res.x, tile_res.y)) {
								Tile* tileDestino = mapa.getTile(tile_res.x, tile_res.y);
								pjm.getPjeLocal().ataque(tileDestino , &mapa);
							} else {
								pjm.getPjeLocal().ataque(NULL , &mapa);
							}
							BitStream bs;
							bs << PROTO::ATACAR << pjm.getPjeLocal().getNick();
							sock.send(bs.str());
							//caminoMinimo.clear();
							break;
						}
						case 'd' : {
							pjm.getPjeLocal().defender(NULL , &mapa);
							BitStream bs;
							bs << PROTO::DEFENDER << pjm.getPjeLocal().getNick();
							sock.send(bs.str());
							//caminoMinimo.clear();
							break;
						}
						case 'f' : {
							//pjm.getPjeLocal().freezar();
							break;
						}
						case 'k' : {
							//pjm.getPjeLocal().muerte();
							//caminoMinimo.clear();
							break;
						}
						case 't' : {
							pjm.getPjeLocal().utilizarTerremoto(&mapa,&pjm, &sock);
							break;
						}
						case 'h' : {
							pjm.getPjeLocal().utilizarHielo(&mapa,&pjm);
							break;
						}
					}
					
				}
				// Mouse clicks
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						vec2<int> tile_res = MouseCoords2Tile(vec2<int>(event.button.x, event.button.y), camara);
						if(mapa.tileExists(tile_res.x, tile_res.y)) {

							Tile* tilePersonaje; 
							//chequeo sicronizacion para que calcule el camino minimo desde el tile al que se esta moviendo al destino
							if (ultimoMovimientoX == NOSEMOVIO && ultimoMovimientoY == NOSEMOVIO){
								tilePersonaje = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
							}else{
								tilePersonaje = mapa.getTile(ultimoMovimientoX,ultimoMovimientoY);
							}
							Tile* tileDestino = mapa.getTile(tile_res.x, tile_res.y);
							//guardo el destino
							ultimoDestinoX = tile_res.x;
							ultimoDestinoY = tile_res.y;
							// Verificamos si hay un personaje para activar el chat
							bool found_pje = false;
							//std::cout << "CLICK @ " << tile_res.x << ";" << tile_res.y << "\n";
							for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
								if(tileDestino == mapa.getTilePorPixeles(it->second.getX(), it->second.getY())) {
									found_pje = true;
									break;
								}
							}

							if(!found_pje) {
								caminoMinimo = mapa.getCaminoMinimo(tilePersonaje, tileDestino);
								indice = 1;
								estadoMovimiento = MOV::MANDAR_POS;
							}

						}
					}

					if(event.button.button == SDL_BUTTON_RIGHT) {
						vec2<int> tile_res = MouseCoords2Tile(vec2<int>(event.button.x, event.button.y), camara);
						if(mapa.tileExists(tile_res.x, tile_res.y)) {

							Tile* tilePersonaje; 
							//chequeo sicronizacion para que calcule el camino minimo desde el tile al que se esta moviendo al destino
							if (ultimoMovimientoX == NOSEMOVIO && ultimoMovimientoY == NOSEMOVIO){
								tilePersonaje = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
							}else{
								tilePersonaje = mapa.getTile(ultimoMovimientoX,ultimoMovimientoY);
							}
							Tile* tileDestino = mapa.getTile(tile_res.x, tile_res.y);
							//guardo el destino
							ultimoDestinoX = tile_res.x;
							ultimoDestinoY = tile_res.y;
							// Verificamos si hay un personaje para activar el chat
							bool found_pje = false;
							//std::cout << "CLICK @ " << tile_res.x << ";" << tile_res.y << "\n";
							for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
								if(tileDestino == mapa.getTilePorPixeles(it->second.getX(), it->second.getY())) {
									found_pje = true;
									if(!it->second.isActivo()) continue; // Si no está activo, no abrimos el chat
									chat_window.setNickDestino(it->second.getNick());
									chat_window.open();
									break;
								}
							}

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
			Tile* t = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(),pjm.getPjeLocal().getY());
			tileActual = make_pair<int,int>(t->getU(), t->getV());
			
			if (puedeMoverse) {				
				if (!choco) {
					std::cout << "Energia: " << (int)pjm.getPjeLocal().getEnergia() << endl;
					std::cout << "Escudo: " << (int)pjm.getPjeLocal().getEnergiaEscudo() << endl;
					std::cout << "Magia: " << (int)pjm.getPjeLocal().getMagia() << endl;
					std::cout << "Velocidad: " << pjm.getPjeLocal().getVelocidad() << endl;
					std::cout << "Terremotos: " << (int)pjm.getPjeLocal().getTerremoto() << endl;
					std::cout << "Hielos: " << (int)pjm.getPjeLocal().getHielo() << endl;
					std::cout << "Radio: " << (int)pjm.getPjeLocal().getRadioY() << endl;
					std::vector<Entidad*> entidades=pjm.getPjeLocal().getPosicion(&mapa)->getEntidades();
					for (auto it=entidades.begin();it!=entidades.end();it++) {
						if ((*it)!=&pjm.getPjeLocal()) //sino choca con si mismo
							(*it)->chocarCon(&pjm.getPjeLocal());
					}
					choco=true;
				}
				//std::cout << puedeMoverse << " " << estadoMovimiento << "\n";
				//si termino de ir al tile anterior
				if(indice < caminoMinimo.size()){
					//establezo proximo tile del camino
					proximoTile = caminoMinimo[indice];
					//verifico que sea caminable
					
					if (mapa.tileExists(proximoTile.first,proximoTile.second) && !(mapa.getTile(proximoTile.first,proximoTile.second)->isCaminable())){
						caminoMinimo.clear();
						estadoMovimiento = MOV::ESPERANDO_OK;
					}
					
					if(estadoMovimiento == MOV::OK_RECV) {
						std::cout << "OK_RECV\n";
						pjm.getPjeLocal().mover(mapa.getTile(proximoTile.first,proximoTile.second));
						choco=false;
						//actualizo posiciones para calcular correctamente el camino minimo
						ultimoMovimientoX = proximoTile.first;
						ultimoMovimientoY = proximoTile.second;
						indice++;
						puedeMoverse = false;
						estadoMovimiento = MOV::MANDAR_POS;
					}else if(estadoMovimiento == MOV::FAIL_RECV) {
						std::cout << "FAIL_RECV\n";
						estadoMovimiento = MOV::MANDAR_POS;
						// Marco el tile como no caminable temporalmente
						Tile* tProx = mapa.getTile(proximoTile.first, proximoTile.second);
						tProx->setNoCaminable();
						// Actualizo el grafo
						mapa.actualizarGrafo(tProx->getU(),tProx->getV());
						// recalculo el camino
						//el camino va desde ultimo tile al que me movi, hasta el que hice click que es el ultimo del camino anterior
						Tile* tilePersonaje = mapa.getTile(ultimoMovimientoX, ultimoMovimientoY);
						
						if(!mapa.tileExists(ultimoMovimientoX, ultimoMovimientoY)) {
							tilePersonaje = mapa.getTile(start_pos_x, start_pos_y);
							//std::cout << "POS PJE: " << pjm.getPjeLocal().getX() << ";" << pjm.getPjeLocal().getY() << "\n";
							std::cout << "TILE PERSONAJE: " << tilePersonaje << "\n";
						}
						std::cout << "ULTIMO DESTINO: " << ultimoDestinoX << ";" << ultimoDestinoY << "\n";
						Tile* tileDestino = mapa.getTile(ultimoDestinoX ,ultimoDestinoY);
						std::cout << "TILE DESTINO - PERSONAJE: " << tileDestino << " - " << tilePersonaje << "\n";
						//calculo el camino
						caminoMinimo.clear();
						caminoMinimo = mapa.getCaminoMinimo(tilePersonaje, tileDestino);
						std::cout << "NUEVO CAMINO MINIMO:\n";
						//for(auto it = caminoMinimo.begin();it != caminoMinimo.end();it++) {
							//std::cout << "- " << it->first << ";" << it->second << "\n";
						//}
						indice = 1;
						// Despues de actualizar el grafo, desmarco el tile
						tProx->setCaminable();
						mapa.actualizarGrafo(tProx->getU(),tProx->getV());
						
					}else if(estadoMovimiento == MOV::ESPERANDO_OK) {
						//std::cout << "ESPERANDO_OK\n";
						// Nada
					}else if(estadoMovimiento == MOV::MANDAR_POS) {
						std::cout << "MANDAR_POS\n";
						estadoMovimiento = MOV::ESPERANDO_OK;
						std::cout << "TILE ACTUAL: " << tileActual.first << ";" << tileActual.second << "\n";
						std::cout << "PROX TILE: " << proximoTile.first << ";" << proximoTile.second << "\n";
						BitStream bs;
						bs << PROTO::REQUEST_POS << proximoTile.first << proximoTile.second;
						sock.send(bs.str());
					}
					//std::cout << "TILE ACTUAL: " << tileActual.first << ";" << tileActual.second << "\n";
					//std::cout << "PROX TILE: " << proximoTile.first << ";" << proximoTile.second << "\n";

				}
			}else{
				if(indice < caminoMinimo.size()) {
					proximoTile = caminoMinimo[indice];
					estadoMovimiento == MOV::MANDAR_POS;
				}else{
					estadoMovimiento == MOV::IDLE;
				}
			}
			estadoPersonaje = pjm.getPjeLocal().update(&mapa);
			
			/*
			if ( (estadoPersonaje == Personaje::MOVER_COMPLETADO) || 
				 (estadoPersonaje == Personaje::ATACAR_COMPLETADO) ||
				 (estadoPersonaje == Personaje::DEFENDER_COMPLETADO) ||
				 (estadoPersonaje == Personaje::FREEZAR_COMPLETADO) ||
				 (estadoPersonaje == Personaje::ESPERANDO_ACCION)){
				puedeMoverse = true;
			}else if ( (estadoPersonaje == Personaje::MOVER_EN_CURSO) || 
				(estadoPersonaje == Personaje::MOVER_ERROR) ||
				(estadoPersonaje == Personaje::FREEZAR_EN_CURSO)
				){
				puedeMoverse = false;
			}*/
			if ((estadoPersonaje == Personaje::MOVER_COMPLETADO)) 
				 {
				puedeMoverse = true;
			}else if ( (estadoPersonaje == Personaje::MOVER_EN_CURSO) || 
				(estadoPersonaje == Personaje::MOVER_ERROR)){
 				puedeMoverse = false;
 			}


			//Piso la señal de estado del personaje
			/*
			Tile* next = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(),pjm.getPjeLocal().getY());
			if (indice>0) {
				if (next->getU()==caminoMinimo[indice-1].first && next->getU()==caminoMinimo[indice-1].second) {
					puedeMoverse=true;
				} else {
					puedeMoverse=false;
				}
			}
			if (indice==0) puedeMoverse=true;
			*/
			// Update a todos los otros personajes
			for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
				it->second.update(&mapa);
			}
			// Update a tiles recorridos
			if(update_recorrido.timer.getTicks() > update_recorrido.INTERVAL) {
				update_recorrido.timer.start();
				auto t = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
				if(t != NULL) {
					update_recorrido.tile_actual = vec2<int>(t->getU(), t->getV());
					//std::cout << "Tile actual: (" << update_recorrido.tile_actual.x << "," << update_recorrido.tile_actual.y << ")\n";
					if(update_recorrido.tile_actual != update_recorrido.tile_anterior) {
						update_recorrido.tile_anterior = update_recorrido.tile_actual;
						BitStream bs;
						bs << PROTO::NIEBLA_SYNC << update_recorrido.tile_actual.x << update_recorrido.tile_actual.y;
						sock.send(bs.str());
					}
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
		mapa.setEntidadesDibujadasFalse(&pjm.getPjeLocal());
		// Dibujamos la ventana de chat
		chat_window.show(screen);
		// Actualizar la pantalla
		SDL_Flip(screen);

		LeaveCriticalSection(&cs_main);
		Sleep(10);

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