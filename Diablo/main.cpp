// SDL Headers
#include "SDL.h"
// Our headers
#include "source/utilities/timer.h"

int main( int argc, char* args[] ) {

	// Ventana de prueba
	SDL_Surface* screen;
    SDL_Init( SDL_INIT_EVERYTHING );
	// Timer de prueba
	Timer test_timer;
	// Init the window
	screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
	SDL_Flip( screen );
	
	test_timer.start();

	// Loopeamos hasta que el timer llegue a 5 segs
	while(test_timer.getTicks() < 5000) {
		SDL_Delay(1); // Hacemos algo en el loop para que no explote el CPU
	}


    SDL_Quit();
    
    return 0;    
}