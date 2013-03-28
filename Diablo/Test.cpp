#include "Test.h"
	
	//Todos los tests
	bool Test::test() {
		bool retorno = true;
		retorno = testImagenEstatica()&& retorno;
		return retorno;
	}

	//Test Imagen Estatica
	bool Test::testImagenEstatica() {
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		//imagen estática a cargar
		ImagenEstatica imagen("../resources/arbol.bmp");
		//retorno del test
		bool retorno = true;
		//primera evaluación de imagen cargada correctamente
		retorno &= (imagen.getSurface() != NULL);
		SDL_Surface* imagenEstatica = imagen.getSurface();
		//Si imagen distinto de nulo
		if (imagenEstatica != NULL) {
		
		} else {
		
		}
		//Actualizar
		SDL_Flip(pantallaDePrueba);
	
		//tiempo de espera
		SDL_Delay( 3000 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);

		return retorno;
	}