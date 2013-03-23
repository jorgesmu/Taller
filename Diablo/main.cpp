// SDL Headers
#include "SDL.h"
// Our headers
#include "source/utilities/timer.h"
#include "source/utilities/surface.h"

int main(int argc, char* args[]) {

	// Ventana de prueba
	SDL_Surface* screen;
    SDL_Init(SDL_INIT_EVERYTHING);
	// Timer de prueba
	Timer test_timer;
	// Surface de prueba
	Surface surf;
	// No tiene manejo de error todavia, es de prueba
	surf.load("test.bmp");
	// Init the window
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	// Draw el bmp
	surf.blit(screen, 50, 50);

	// Actualizar la pantalla
	SDL_Flip(screen);
	// Arranca el timer
	test_timer.start();

	// Loopeamos hasta que el timer llegue a 5 segs
	while(test_timer.getTicks() < 5000) {
		SDL_Delay(1); // Hacemos algo en el loop para que no explote el CPU
	}

	surf.destroy();

    SDL_Quit();
    
    return 0;    
}