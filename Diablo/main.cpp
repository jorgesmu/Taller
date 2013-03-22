#include "SDL.h"

int main( int argc, char* args[] )
{

	SDL_Surface* screen;
    SDL_Init( SDL_INIT_EVERYTHING );

	screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
	SDL_Flip( screen );
	SDL_Delay( 2000 );

    SDL_Quit();
    
    return 0;    
}