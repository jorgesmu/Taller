#include "texto.h"
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 32

//Constructor de la clase
Texto::Texto() {
	background=NULL;
	message=NULL;
	screen=NULL;
	font=NULL;
}

//Setea el color del texto
void Texto::setColor(int R, int G, int B) {
	this->textColor.r = R;
	this->textColor.g = G;
	this->textColor.b = B;
}

//Funcion de prueba
int Texto::test() {
	std::cout << "Start\n";
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) {
		std::cout << "Error en SDL_Init()\n";
        return 1;    
    }
	if(!init("Prueba TTF")) {
		std::cout << "Error en init()\n";
		return 1;
	}
	setColor(255,255,255);
	
	if(!cargarArchivos("../resources/imagenFondo.bmp", 24, "../resources/lazy.ttf")) {
		std::cerr << "Error cargando archivos\n";
		return 1;
	}else{
		std::cout << "Cargado!\n";
	}
	//Render the text
	SDL_Surface* tempMessage=TTF_RenderText_Solid( font, "The quick brown fox jumps over the lazy dog", textColor );
    
    //If there was an error in rendering the text
    if( tempMessage == NULL )
    {
        return 1;    
    }
    message = new Surface();
	this->message->setSurface(tempMessage);
    //Apply the images to the screen
	this->background->blit(screen,0,0);
	this->message->blit(screen,0,150);
    
    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;    
    }
	int a;
	std::cin >> a;
	destruir();
	SDL_Quit();
	return 0;	
}

//Inicializa el sistema en un estado valido
bool Texto::init(char* caption){    
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    
    if( screen == NULL ) {
        return false;    
    }

    if( TTF_Init() == -1 ) {
        return false;    
    }
    SDL_WM_SetCaption(caption, NULL );

    return true;
}

//Carga los archivos correspondientes para la fuente
bool Texto::cargarArchivos(const char* bkg,int size,const char* nombreFont){
	std::cout << "STUFF\n";
	this->background = new Surface();
	if (!this->background->load(bkg)) {
		std::cerr << "error loading bg\n";
		return false;
	}
    font = TTF_OpenFont(nombreFont, size);

	if (font == NULL) {
		std::cerr << "error load font\n";
		return false;
	}
    
    return true;    
}

//Destruye las estructuras
void Texto::destruir() {
	//Free the surfaces
	this->background->destroy();
	this->message->destroy();
    
    //Close the font that was used
    TTF_CloseFont( font );
    
    //Quit SDL_ttf
    TTF_Quit();
    
}
