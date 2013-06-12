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
#include "../../source/utilities/soundman.h"
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
#include "../../source/utilities/interface.h"
#include "../../source/utilities/bolaDeCristal.h"
#include "../../source/utilities/GolemItem.h"
#include "../../source/utilities/Botella.h"
#include "../../source/utilities/Corazon.h"
#include "../../source/utilities/Hielo.h"
#include "../../source/utilities/bandera.h"
#include "../../source/utilities/arma.h"
#include "../../source/display/boton.h"
#include "../../source/utilities/console.h"
using namespace std;

logErrores err_log("log_cliente.txt");
bool pasoArchivos = false;
bool cargoMapa = false;
const int NOSEMOVIO = -1;
// Variables globales
// Critical section
CRITICAL_SECTION cs_main;
SDL_Event event;
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
bool init_bolaDeCristal, init_golem;
char init_energia,init_magia,init_escudo,init_terremoto,init_hielo,init_bombas;
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
// User Interface
Interface ui;
// Consola de mensajes
Console consola;
// ResMan
ResMan resman;
// Sound manager
SoundMan soundman;
// Config
config_general configuracion;
//Choques
bool choco;
//Muerte que llega desde el personaje como aviso
//bool murio;
// Flags ataque
bool enAtaque = false; // indica si se encuentra en ataque
bool calcularAtaque = false; // indica si se debe calcular el camino minimo para un ataque
// Variables especiales para ataque
Personaje* personajeObjetivo = NULL;
Tile* tilePersonajeObjetivo =  NULL;
//Socket
ClientSocket sock;
// Cargo las entidades en un vector
std::vector<EntidadFija*> entidades_cargadas;

int main(int argc, char* argv[]) {
	// Verificamos que se pase el nick y el tipo
	if(argc != 3) {
		std::cout << "Falta especificar nick:\ncliente.exe <nick> <tipo_personaje>\n";
		pje_local_nick = "jugador";
		pje_local_tipo = "soldado";
		//return 0;
	}else{
		// Cargamos el nick y tipo de la consola
		pje_local_nick = argv[1];
		pje_local_tipo = argv[2];
	}

	// Ventana de prueba
	SDL_Surface* screen;
	putenv("SDL_VIDEO_CENTERED=1"); // Para centrar la ventana
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) { std::cerr << "Error @ SDL_Init(): " << SDL_GetError() << "\n"; return -1; }
	// Init the window 
	// tamaño de pantalla hardcodeado
	//screen = SDL_SetVideoMode(pantalla->get_ancho(), pantalla->get_alto(), 32, SDL_SWSURFACE);
	screen = SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE);
	// Resman
	if(!resman.init()) return -2;
	resman.addRes("boton_sp", "../resources/static/boton_sp.png");
	resman.addRes("boton_mp", "../resources/static/boton_mp.png");
	// Para confinar el mouse a la ventana
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	// Lo movemos al medio
	SDL_WarpMouse(800/2, 600/2);
	SDL_WM_SetCaption("Diablo", NULL);

	// MENU
	//////////////////////////
	Boton sp, mp;
	sp.init(&resman, 10, 10, "boton_sp");
	mp.init(&resman, 10, 40, "boton_mp");
	int opcion_menu = -1;
	// 1 = sp, 2 = mp, 3 = exit
	while(opcion_menu == -1) {
		SDL_PollEvent(&event);
		if(sp.handleInput(event)) opcion_menu = 1;
		if(mp.handleInput(event)) opcion_menu = 2;
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) opcion_menu = 3;
		sp.show(screen);
		mp.show(screen);
		SDL_Flip(screen);
	}
	//std::cout << "OPCION MENU: " << opcion_menu << "\n";

	if(opcion_menu == 1) {
		// Single player
		ShellExecute(NULL, "open", "servidor.exe", NULL, NULL, SW_SHOWMINNOACTIVE);
	}else if(opcion_menu == 2) {
		// Multiplayer, no hace nada
	}else if(opcion_menu == 3) {
		// Salir
		return 1;
	}


	// FIN DE MENU
	//////////////////////////

	InitializeCriticalSection(&cs_main);
	// Socket de cliente
	if(!sock.init(&cs_main)) {
		return 1;
	}

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

	mapa.resize(escenarios[escenario_elegido_id].get_tam_x(), escenarios[escenario_elegido_id].get_tam_x());

	// Camara
	Camara camara;
	camara.init(pantalla->get_ancho(), pantalla->get_alto(), configuracion.get_margen_scroll(), mapa);
	
	// SoundMan
	if(!soundman.init(&camara, &pjm.getPjeLocal())) return -3;
	soundman.addSound("sword", "../resources/static/sounds/sword.wav");

	// Ventana de chat
	chat_window.init(&resman, 40, 40, Font::SIZE_NORMAL, 250, 500, COLOR::WHITE);
	chat_window.setNickLocal(pje_local_nick);

	// Consola
	consola.init(&resman, 10, 10, 200, Font::SIZE_NORMAL, COLOR::WHITE);

	resman.addRes("bandera","../resources/bandera.png");
	// Cargo la entidad por default
	resman.addRes("tierraDefault", "../resources/tile.png");
	Entidad entidadPisoPorDefecto("tierraDefault", 1 , 1 , true , 0 , 0 , NULL, resman , Imagen::COLOR_KEY);
	
	// Cargamos el tile por defecto
	for(auto it = mapa.allTiles().begin();it != mapa.allTiles().end(); ++it) {
		if(it->sinEntidades()) {
			it->addEntidad((Entidad*)&entidadPisoPorDefecto);
		}
	}	

	//Recursos para armas
	resman.addRes("bomba","../resources/bomba.jpg");

	//Prueba de carga items
	resman.addRes("cofre","../resources/cajaEstatica.png");
	/*GolemItem cofre("cofre",1,1,true, 6 ,13,NULL,&mapa,resman,Imagen::COLOR_KEY );
	mapa.getTile(6,13)->addEntidad(&cofre,&mapa);
	entidades_cargadas.push_back(&cofre);*/
	/*
	resman.addRes("cofre","../resources/bandera.png");
	Bandera cofre("cofre",1,1,true, 6 ,13,NULL,&mapa,resman,Imagen::COLOR_KEY );
	mapa.getTile(6,13)->addEntidad(&cofre,&mapa);
	entidades_cargadas.push_back(&cofre);
	*/


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
	//--------------------------------------------------------------------------------------------------
	// Arma
	resman.addRes("espada","../resources/espada.png");

	//--------------------------------------------------------------------------------------------------

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
		std::cout << "Restaurando atributos..." << endl;
		// El server me paso atributos para cargar
		pjm.getPjeLocal().setVelocidad(init_vel);
		pjm.getPjeLocal().setEnergia(init_energia);
		pjm.getPjeLocal().setMagia(init_magia);
		pjm.getPjeLocal().setEnergiaEscudo(init_escudo);
		pjm.getPjeLocal().setTerremoto(init_terremoto);
		pjm.getPjeLocal().setHielo(init_hielo);
		pjm.getPjeLocal().setRadio(init_radio);
		pjm.getPjeLocal().setBolaDeCristal(init_bolaDeCristal);
		pjm.getPjeLocal().setGolem(init_golem);
		pjm.getPjeLocal().setCantBombas(init_bombas);
	} else {
		//Aviso al server mis valores por defecto de atributos
		std::cout << "Avisando al server de mis attributos default..." << endl;
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
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::BOLA_DE_CRISTAL << pjm.getPjeLocal().getBolaDeCristal();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::GOLEM << pjm.getPjeLocal().tieneGolem();
		sock.send(bs.str());
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::CANT_BOMBAS << pjm.getPjeLocal().getCantBombas();
		sock.send(bs.str());
	}
	
	//std::cout << "valor del golem: " << pjm.getPjeLocal().tieneGolem() << "\n";
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
								Personaje* personaje = NULL;
								bool found_pje = false;
								for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
									if(tileDestino == mapa.getTilePorPixeles(it->second.getX(), it->second.getY())) {
										found_pje = true;
										personaje= &(it -> second);
										break;
									}
								}
								if (found_pje){
									if (personaje->getNick() != pjm.getPjeLocal().getNick()) {
										pjm.getPjeLocal().ataque(tileDestino , &mapa , personaje);
									}
								}
							} else {
								pjm.getPjeLocal().ataque(NULL , &mapa);
							}
							
							//BitStream bs;
							//bs << PROTO::ATACAR << pjm.getPjeLocal().getNick();
							//sock.send(bs.str());
							//caminoMinimo.clear();
							break;
						}
						case 'b' : {
							if (pjm.getPjeLocal().getCantBombas()>0) {
								int x = pjm.getPjeLocal().getPosicion(&mapa)->getU();
								int y = pjm.getPjeLocal().getPosicion(&mapa)->getV();
								pjm.getPjeLocal().utilizarBomba(x,y);
							}
							break;
						}
						case 'g' : {
							if (pjm.getPjeLocal().tieneGolem()) {
								pjm.getPjeLocal().utilizarGolem();
							}
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
						case 'x' : {
							if (pjm.getPjeLocal().tieneTransmutacion()) {
								//Veo si hay un enemigo en algun tile adyacente
								int deltaX,deltaY;
								int otraPosX,otraPosY,miPosX,miPosY;
								miPosX = pjm.getPjeLocal().getPosicion(&mapa)->getU();
								miPosY = pjm.getPjeLocal().getPosicion(&mapa)->getV();
								bool enemigoCerca = false;
								for (auto it = pjm.getPjes().begin(); it != pjm.getPjes().end(); it++) {
									otraPosX = it->second.getPosicion(&mapa)->getU();
									otraPosY = it->second.getPosicion(&mapa)->getV();
									deltaX = std::abs(miPosX-otraPosX);
									deltaY = std::abs(miPosY-otraPosY);
									if (deltaX <= 1 && deltaY <= 1) {
										pjm.getPjeLocal().utilizarTransmutacion(it->first);
									}
								}
								
							}
							break;
						}
					}
					
				}
				// Mouse clicks
				if(event.type == SDL_MOUSEBUTTONDOWN) {
					if(event.button.button == SDL_BUTTON_LEFT) {
						// ante un click el ataque se anula
						personajeObjetivo = NULL;
						enAtaque = false;
						tilePersonajeObjetivo = NULL;
						calcularAtaque = false;
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
							Personaje* personajeAux = NULL;
							//std::cout << "CLICK @ " << tile_res.x << ";" << tile_res.y << "\n";
							for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
								if(tileDestino == mapa.getTilePorPixeles(it->second.getX(), it->second.getY())) {
									found_pje = true;
									personajeAux =  &(it->second);
									break;
								}
							}
							if(!found_pje) {
								caminoMinimo = mapa.getCaminoMinimo(tilePersonaje, tileDestino);
								indice = 1;
								estadoMovimiento = MOV::MANDAR_POS;
								enAtaque = false;
								calcularAtaque = false;
							} else if  (personajeAux -> getNick() != pjm.getPjeLocal().getNick()){
								calcularAtaque = true;
								tilePersonajeObjetivo = tileDestino;
								personajeObjetivo = personajeAux;
							}
						}
					}

					if(event.button.button == SDL_BUTTON_RIGHT) {
						vec2<int> tile_res = MouseCoords2Tile(vec2<int>(event.button.x, event.button.y), camara);
						if(mapa.tileExists(tile_res.x, tile_res.y)) {

							Tile* tilePersonaje; 
							//chequeo sicronizacion para que calcule el camino minimo desde el tile al que se esta moviendo al destino
							if (ultimoMovimientoX == NOSEMOVIO && ultimoMovimientoY == NOSEMOVIO){
								tilePersonaje = mapa.getTile(pjm.getPjeLocal().getX(), pjm.getPjeLocal().getY());
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

		// calculo de ataque
		if (calcularAtaque) {
			printf("\nCalculo del camino de ataque %s\n", pjm.getPjeLocal().getNick().c_str());
			if (personajeObjetivo != NULL) {
				Tile* tileActualLocal = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(),pjm.getPjeLocal().getY());
				tilePersonajeObjetivo = mapa.getTilePorPixeles(personajeObjetivo->getX(),personajeObjetivo->getY());
				if ((tileActualLocal != NULL) && (tilePersonajeObjetivo != NULL)){
					caminoMinimo = mapa.getCaminoMinimo(tileActualLocal,tilePersonajeObjetivo);
					indice = 1;
					estadoMovimiento = MOV::MANDAR_POS;
					caminoMinimo.pop_back();
					enAtaque = true;
					std::cout << "enAtaque = true\n";
				}else {
					enAtaque = false;
					personajeObjetivo = NULL;
					tilePersonajeObjetivo = NULL;
				}
			} else {
				enAtaque = false;
				personajeObjetivo = NULL;
			}
			calcularAtaque = false;
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
			// Veo si me tengo que relocalizar en el mapa
			if (estadoMovimiento == MOV::OK_REV_RECV) {
				std::cout << "Relocalizando al jugador..." << endl;
				int x = pjm.getPjeLocal().getPosicion(&mapa)->getU();
				int y = pjm.getPjeLocal().getPosicion(&mapa)->getV();
				// Lo elimino de la posicion donde murio
				mapa.getTile(pjm.getPjeLocal().getPosicion(&mapa)->getU(), pjm.getPjeLocal().getPosicion(&mapa)->getV())->deleteEntidad(&(pjm.getPjeLocal()));
				// Posiciono el personaje
				mapa.getTile(start_pos_x, start_pos_y)->addEntidad(&(pjm.getPjeLocal()));
				pjm.getPjeLocal().setTileActual(mapa.getTile(start_pos_x, start_pos_y));
				// Inicializo el recorridor
				update_recorrido.tile_anterior = vec2<int>(start_pos_x, start_pos_y);
				update_recorrido.tile_actual = vec2<int>(start_pos_x, start_pos_y); 
				update_recorrido.timer.start();
				// Cosas del movimiento
				estadoMovimiento = MOV::IDLE;
				// Centro la camara
				camara.center(mapa.getTile(start_pos_x, start_pos_y)->getX(), mapa.getTile(start_pos_x, start_pos_y)->getY());
				pjm.getPjeLocal().revivir();
				ultimoMovimientoX=NOSEMOVIO;
				ultimoMovimientoY=NOSEMOVIO;

				//meto el item que deja cuando muere
				int cant_items = 9;//cantidad de items que implementamos
				int rand;
				rand = intRand(0, cant_items);
				Tile* tile = mapa.getTile(x,y);
				Item* item;
				rand = 0;
				switch(rand){
					case 0:{
						item = new Lampara("cofre",1,1,true, -13 , 20, tile,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::LAMPARA << x << y;
						sock.send(bs.str());
						break;
					}
					case 1:{
						item = new MapaItem ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::MAPAITEM << x << y;
						sock.send(bs.str());
						break;
					}
					case 2:{
						item = new Zapatos ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::ZAPATOS << x << y;
						sock.send(bs.str());
						break;
					}
   					case 3:{
						item = new Terremoto ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::TERREMOTO << x << y;
						sock.send(bs.str());
						break;
					}
   					case 4:{
						item = new Escudo ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::ESCUDO << x << y;
						sock.send(bs.str());
						break;
					}
   					case 5:{
						item = new BolaDeCristal ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::BOLA_DE_CRISTAL << x << y;
						sock.send(bs.str());
						break;
					}
   					case 6:{
						item = new GolemItem ("cofre",1,1,true, 6 ,13,NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::GOLEM << x << y;
						sock.send(bs.str());
						break;
					}
   					case 7:{
						item = new Botella ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::BOTELLA << x << y;
						sock.send(bs.str());
						break;
					}
   					case 8:{
						item = new Corazon ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::CORAZON << x << y;
						sock.send(bs.str());
						break;
					}
   					case 9:{
						item = new Hielo ("cofre",1,1,true, x , y, NULL,&mapa,resman,Imagen::COLOR_KEY );
						mapa.getTile(x, y)->addEntidad(item,&mapa);
						entidades_cargadas.push_back(item);
						BitStream bs;
						bs << PROTO::LEAVE_ITEM << ITEM::HIELO << x << y;
						sock.send(bs.str());
						break;
					}
   					/*case 10:{
						ArmaBomba cofre("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
					}*/
				}
			}

			// Actualiza la camara
			camara.update();
			// Actualiza las entidades
			for(auto it = entidades_cargadas.begin(); it != entidades_cargadas.end(); ++it){
				(*it)->update(&mapa);
			}
			// Actualizamos los personajes
			for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end(); it++) {
				int control;
				control = it->second.update(&mapa);
				//reseteo update de personajes por si se freno
				if(it->second.get_timer_update().getTicks() > 500)
						it->second.set_posicion_actualizada(false);
				if (control == Personaje::MOVER_COMPLETADO && it->second.get_posicion_actualizada()==false){
					//aviso al server que se termino de mover un personaje para si es un enemigo actualizarlo
					Tile* unTile = it->second.getPosicion(&mapa);
					//cout << "pos " << unTile->getU() << "," <<unTile->getV() <<endl;
					bs.clear();
					bs << PROTO::EN_MOVE_CMPLT << it->second.getNick() << unTile->getU() << unTile ->getV() ;
					sock.send(bs.str()); 
					//std::cout << "Mandando termino de moverse personaje a servidor" << it->second.getNick() << "\n";
					//estado de personaje es para el pje local y estoy en un loop de otros personajes
					//estadoPersonaje = Personaje::ESPERANDO_ACCION;
					bs.clear();
					it->second.set_posicion_actualizada(true);
					it->second.get_timer_update().start();
				}
			}
			// Actualizamos el personaje principal
			Tile* t = mapa.getTilePorPixeles(pjm.getPjeLocal().getX(),pjm.getPjeLocal().getY());
			tileActual = make_pair<int,int>(t->getU(), t->getV());

			//Para revivir
			pjm.getPjeLocal().updateRevivir();

			//Para que exploten bombas
			pjm.getPjeLocal().updateBomba();

			//Para que termine el hechizo hielo
			pjm.getPjeLocal().updateHielo();

			//Para que vuelva atras la transmutacion
			pjm.getPjeLocal().updateTransmutacion();
			
			if (puedeMoverse) {				
				if (!choco) {
					std::vector<Entidad*> entidades=pjm.getPjeLocal().getPosicion(&mapa)->getEntidades();
					std::list<Entidad*> entidadesChocadas; //asi evito chocar dos veces, con la entidad y con el ancla
					bool yaChoco=false;
					entidadesChocadas.push_back(&pjm.getPjeLocal()); //sino choca con si mismo
					for (auto it=entidades.begin();it!=entidades.end();it++) {
						//Veo si ya choque con esta entidad
						for (auto it2=entidadesChocadas.begin(); it2!=entidadesChocadas.end(); it2++) {
							if ((*it2)==(*it)) yaChoco=true;
						}
						if (!yaChoco) {
							(*it)->chocarCon(&pjm.getPjeLocal());
							entidadesChocadas.push_back(*it);
						}
					}
				}
				choco=true;
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
						//std::cout << "OK_RECV\n";
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
						//std::cout << "MANDAR_POS\n";
						estadoMovimiento = MOV::ESPERANDO_OK;
						//std::cout << "TILE ACTUAL: " << tileActual.first << ";" << tileActual.second << "\n";
						//std::cout << "PROX TILE: " << proximoTile.first << ";" << proximoTile.second << "\n";
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
				// verifico si se encuentra en condiciones de atacar
				if (enAtaque) {
					// verifico que el personaje se encuentre en tile sobre el cual calcule el camino minimo
					if ((personajeObjetivo != NULL) && (tilePersonajeObjetivo != NULL)){
						Tile* tileActualObjetivo = mapa.getTilePorPixeles(personajeObjetivo->getX(),personajeObjetivo->getY());
						if (tileActualObjetivo != NULL){
							// si no lo esta vuelvo a calcular el camino
							if (tileActualObjetivo != tilePersonajeObjetivo){
								printf("\nSe determino que se debe recalcular camino %s\n",pjm.getPjeLocal().getNick().c_str());
								calcularAtaque = true;
							} else {
								// verifico si se llego al final de camino minimo
								if (indice == caminoMinimo.size()){
									pjm.getPjeLocal().ataque(tilePersonajeObjetivo,&mapa,personajeObjetivo);
									enAtaque = false;
									personajeObjetivo = NULL;
									tilePersonajeObjetivo = NULL;
									printf("\nAtaque Finalizado %s\n",pjm.getPjeLocal().getNick().c_str());
								}
							}
						} else{
							enAtaque = false;
							tilePersonajeObjetivo = NULL;
							personajeObjetivo = NULL;
						}
					}
				}
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

			// Update a todos los otros personaje
		/*	for(auto it = pjm.getPjes().begin();it != pjm.getPjes().end();it++) {
				int estadoPersonaje;
				estadoPersonaje = it->second.update(&mapa);
				if (estadoPersonaje == Personaje::MOVER_COMPLETADO){
					//aviso al server que se termino de mover un personaje para si es un enemigo actualizarlo

/*					Tile* unTile = it->second.getPosicion(&mapa);
					//cout << "pos "<< unTile->getU() << "," <<unTile->getV();
					bs.clear();
					bs << PROTO::EN_MOVE_CMPLT << it->second.getNick() << unTile->getU() << unTile ->getV() ;
					sock.send(bs.str());
					std::cout << "Mandando termino de moverse personaje a servidor" << it->second.getNick() << "\n";
					estadoPersonaje = Personaje::ESPERANDO_ACCION;
					bs.clear();
				
				}
			}*/
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
			// Update a la consola
			consola.update();
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
		// Dibujamos la consola
		consola.show(screen);

		ui.blitInterface(screen);

		// Actualizar la pantalla
		SDL_Flip(screen);

		LeaveCriticalSection(&cs_main);
		Sleep(10);

	}
	
	mapa.clean();
	resman.clean();
	soundman.clean();
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