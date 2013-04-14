// SDL Headers
#include "SDL.h"
// Our headers
#include "source/utilities/timer.h"
#include "source/utilities/surface.h"
#include "source/utilities/aux_func.h"
#include "source/utilities/PruebasParser.h"
#include "source/utilities/parser.h"
#include "source/display/entidad.h"
#include "source/utilities/Personaje.h"
#include "source/display/entidadFija.h"
#include "source/display/camara.h"
#include "source/display/mapa.h"
#include "source/display/resman.h"
#include "source/constants/model.h"
#include "source/utilities/Test.h"
#include "source/utilities/coordenadas.h"

int main(int argc, char* args[]) {

	// Parseo el nivel
	config_juego juego = parsear("../resources/levels/nivel1.yaml");
	config_pantalla* pantalla = juego.get_pantalla();
	vector <config_entidad> entidades = juego.get_entidades();
	config_general configuracion = juego.get_configuracion();
	vector <config_escenario> escenarios = juego.get_escenarios();

	// Ventana de prueba
	SDL_Surface* screen;
    SDL_Init(SDL_INIT_EVERYTHING);
	// Para confinar el mouse a la ventana
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_WarpMouse(800/2, 600/2);
	// Init the window
	screen = SDL_SetVideoMode(pantalla->get_ancho(), pantalla->get_alto(), 32, SDL_SWSURFACE);
	// Camara
	Camara camara;
	camara.init(pantalla->get_ancho(), pantalla->get_alto(), configuracion.get_margen_scroll());

	// Mapa
	Mapa mapa;
	mapa.resize(escenarios[0].get_tam_x(), escenarios[0].get_tam_x());
	// Resman
	ResMan resman;
	resman.init();

	// Cargo las entidades en un vector
	std::vector<Entidad*> entidades_cargadas;
	for (auto it = entidades.begin(); it != entidades.end(); ++it){
		resman.addRes(it->get_nombre(), it->get_path_imagen(), Imagen::COLOR_KEY);
		Entidad* entidad;
		if(it->get_fps() == -1){
			entidad = new EntidadFija (it->get_nombre(), it->get_ancho_base(), it->get_alto_base(), it->get_pixel_ref_x(), it->get_pixel_ref_y(),
								NULL, &mapa, resman, Imagen::COLOR_KEY);
		}else{
			entidad = new Entidad (it->get_nombre(), it->get_ancho_base(), it->get_alto_base(), it->get_fps(), it->get_delay(),
								it->get_pixel_ref_x(), it->get_pixel_ref_y(), NULL, resman, Imagen::COLOR_KEY);		
		}
		entidades_cargadas.push_back(entidad);
	}

	// Cargo la entidad por default
	resman.addRes("tierraDefault", "../resources/tile.bmp");
	Entidad entidadPisoPorDefecto("tierraDefault", 1 , 1 , 0 , 0 , NULL, resman , Imagen::COLOR_KEY);

	// Vector de entidades en este mapa
	vector<config_entidad_en_juego> entidades_en_juego = escenarios[0].get_entidades();
	
	// Revisamos que no haya quedado ningun tile sin entidades
	// Si hay alguno le ponemos la entidad por defecto
	for(auto it = mapa.allTiles().begin();it != mapa.allTiles().end(); ++it) {
		if(it->sinEntidades()) {
			it->addEntidad((Entidad*)&entidadPisoPorDefecto);
		}
	}

	// Llenamos el mapa con las entidades
	for(auto it = entidades_en_juego.begin(); it != entidades_en_juego.end(); ++it){
		bool entidad_encontrada = false;
		for(auto it2 = entidades_cargadas.begin(); it2 != entidades_cargadas.end(); ++it2){
			if(it->get_nombre() == (*it2)->get_nombre()){
				mapa.getTile(it->get_pos_x(), it->get_pos_y())->addEntidad(*it2);
				(*it2)->setTileActual(mapa.getTile(it->get_pos_x(), it->get_pos_y()), &mapa);
				entidad_encontrada = true;
				break;
			}
		}
		if(!entidad_encontrada) {
			std::cerr << "Entidad " << it->get_nombre() << " no fue cargada porque no fue definida\n";
		}
	}
	 


	// Agrega el personaje
	Personaje personaje(escenarios[0].get_protagonista().get_nombre() , 1 , 1 , 50 , 5, 100, 100 ,	configuracion.get_vel_personaje(),	0 , 70 , NULL , resman , Imagen::COLOR_KEY);
	mapa.getTile(escenarios[0].get_protagonista().get_pos_x(), escenarios[0].get_protagonista().get_pos_y()) ->addEntidad(&personaje);

	// Variables para el game-loop
	double curr_time = SDL_GetTicks();
    double accum = 0.0;
	bool quit = false;

	// Para guardar los eventos  de input
	SDL_Event event;

	while(!quit) {

		// Input handling (esto despues se movera a donde corresponda)
		while(SDL_PollEvent(&event)) {
			// Detectar escape o quit
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) {
				quit = true;
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
						personaje.mover(mapa.getTile(tile_res.x,tile_res.y));
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
			// Actualiza el personaje
			personaje.update(&mapa);
			// Decrease al accum
			accum -= CONST_DT;
		}

		// Aca se hace el blitteo
		// Despues se movera a donde corresponda
		SDL_FillRect(screen, NULL, 0);
		// Draw el mapa
		mapa.blit(screen, camara);
		// Actualizar la pantalla
		SDL_Flip(screen);
	}
	
	mapa.clean();
	resman.clean();

    Test::test();

	SDL_Quit();
	 
    return 0;    
}