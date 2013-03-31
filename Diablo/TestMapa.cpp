// SDL Headers
#include "SDL.h"
// Our headers
#include "source/utilities/timer.h"
#include "source/utilities/surface.h"
#include "source/utilities/aux_func.h"
#include "source/utilities/parser.h"

#include "source\display\camara.h"
#include "source\display\mapa.h"
#include "source/display/tile.h"

#include "source/constants/model.h"

//#include "Test.h"

#include <vector>

void mapa_test(){
	SDL_Surface* screen;
    SDL_Init(SDL_INIT_EVERYTHING);
	// Para confinar el mouse a la ventana
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_WarpMouse(640/2, 480/2);
	// Init the window
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	// Camara
	Camara camara;
	camara.init(640, 480, 30);
	// Surface de prueba
	//Surface surf;
	// No tiene manejo de error todavia, es de prueba
	//surf.load("../resources/tile.bmp" , Surface::RGB_AZUL);

	Entidad entidad;
	entidad.init("cemento", "../resources/test.bmp");
	Tile tile;
	tile.addEntidad(entidad);

	SDL_Event event;
	bool quit = false;

	SDL_FillRect(screen, NULL, 0);
	while(!quit){
		while(SDL_PollEvent(&event)) {
			//Detectar escape o quit
			if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) {
				quit = true;
			}
			//Detectar mouse motion
			if(event.type == SDL_MOUSEMOTION) {
				// Update para la camara
				camara.update_speed(makeRect(event.motion.x, event.motion.y));
			}
		}
		
		int mapa_alto, mapa_ancho;
		mapa_alto = 480/10;
		mapa_ancho = 640/20;

		for(int y = 0; y < mapa_alto; y++){
			for(int x = 0; x < mapa_ancho; x++){

				int px = (x - y)*(tile.getAncho()/2);
				int py = (x + y)*(tile.getAlto()/2);

				tile.blit(screen, makeRect(px , px));
				SDL_Flip(screen);
			}
		}
	}
}