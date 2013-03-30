// SDL Headers
#include "SDL.h"
// Our headers
#include "source/utilities/timer.h"
#include "source/utilities/surface.h"
#include "source/utilities/aux_func.h"
#include "source/utilities/parser.h"

#include "source\display\camara.h"

#include "source/constants/model.h"
#include "Test.h"

int main(int argc, char* args[]) {

	//Prueba parser YAML
	parser_test();

	// Ventana de prueba
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
	Surface surf;
	// No tiene manejo de error todavia, es de prueba
	surf.load("../resources/test.bmp" , Surface::RGB_AZUL);

	double curr_time = SDL_GetTicks();
    double accum = 0.0;
	bool quit = false;

	// Para guardar los eventos
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
			
			camara.update();

			accum -= CONST_DT;
		}

		// Aca se hace el blitteo
		// Despues se movera a donde corresponda
		SDL_FillRect(screen, NULL, 0);
		// Draw el bmp
		surf.blit(screen,(int) (50 - camara.getX()), (int) (50 - camara.getY())); // Este usa coordenadas relativas basadas en la camara
		surf.blit(screen, 300, 300, makeRect(115, 0, surf.width(), surf.height())); // Este no

		// Actualizar la pantalla
		SDL_Flip(screen);

	}

	//Test::test();
	
	surf.destroy();

    SDL_Quit();

 
    return 0;    
}