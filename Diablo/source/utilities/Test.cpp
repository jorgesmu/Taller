#include "Test.h"
#include "../display/entidad.h"


	//Todos los tests
	bool Test::test() {
		bool retorno = true;
		//retorno &= testSurface();
		//retorno &= testImagenEstatica();
		//retorno &= Test::testImagenAnimada();
		//retorno &= Test::testImagenPersonaje();
		return retorno;
	}
	
	//Test Surface
	bool Test::testSurface() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		//surface
		Surface surface;
		//surface con nueva configuracion
		surface.nuevoSurfaceConfigurado(100 , 100 , SDL_GetVideoInfo() , Surface::BMP_TRANSPARENCIA );
		if (surface.getSDL_Surface() != NULL) {
			SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE );
			SDL_FillRect(surface.getSDL_Surface() , NULL , Surface::RGB_AZUL); 
			surface.blit(pantallaDePrueba , 0 , 0);
		} else {
			// Se pone roja si hay error
			SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_ROJO);
			retorno = false;
		}
		//Actualizar
		SDL_Flip(pantallaDePrueba);
	
		//tiempo de espera
		SDL_Delay( 500 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		
		return retorno;
	}

	//Test Imagen Estatica
	bool Test::testImagenEstatica() {
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		//imagen estática a cargar
		ImagenEstatica imagen("../resources/arbol.bmp" , Surface::BMP_TRANSPARENCIA);
		//retorno del test
		bool retorno = true;
		//primera evaluación de imagen cargada correctamente
		retorno &= (imagen.getSurface() != NULL);
		Surface* imagenEstatica = imagen.getSurface();
		//Si imagen distinto de nulo
		if (imagenEstatica -> getSDL_Surface() != NULL) {
			for (int i = 0 ; i < 10 ; i++) {
				imagenEstatica -> blit(pantallaDePrueba , 0 , 0);
				imagenEstatica = imagen.getSurface();
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 200 );
			}
		} else {
			retorno = false;
		}
		//Actualizar
		SDL_Flip(pantallaDePrueba);
	
		//tiempo de espera
		SDL_Delay( 100 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);

		return retorno;
	}

	//Test Imagen Animada
	bool Test::testImagenAnimada() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		
		//imagen animada a cargar
		ImagenAnimada imagen("../resources/Soldado.bmp" , 100 , 100 , 10 , 500 
								, Surface::BMP_TRANSPARENCIA);
				
		//primera evaluación de imagen cargada correctamente
		
		Surface* surfaceImagenAnimada = imagen.getSurface();
		retorno &= (surfaceImagenAnimada != NULL);
		//Si imagen distinto de nulo
		if (surfaceImagenAnimada != NULL) {
			for (int i = 0 ; i < 300 ; i++) {
				SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
				surfaceImagenAnimada -> blit(pantallaDePrueba , 0 , 0);
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 100 );
				surfaceImagenAnimada = imagen.getSurface();
			}

		} else {
			SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_ROJO);
			retorno = false;
		}
		
		//Actualizar
		SDL_Flip(pantallaDePrueba);
	
		//tiempo de espera
		SDL_Delay( 200 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		
		return retorno;
	}

	//Test Imagen Personaje
	bool Test::testImagenPersonaje() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		
		//imagen animada a cargar
		ImagenPersonaje imagen("../resources/Soldado.bmp" , 100 , 100 , 10 , 500 
								, Surface::BMP_TRANSPARENCIA);
				
		//primera evaluación de imagen cargada correctamente
		
		Surface* surfaceImagenPersonaje = imagen.getSurface();
		retorno &= (surfaceImagenPersonaje != NULL);
		//Si imagen distinto de nulo
		if (surfaceImagenPersonaje != NULL) {
			
			for (unsigned int accion = ImagenPersonaje::DES_SUR ; accion <= ImagenPersonaje::MUERTE ; accion++) {
				for(int volver = 1 ; volver < accion ; volver++) {
					imagen.setAccion(accion);
					for (int i = 0 ; i < 12 ; i++) {
						SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
						surfaceImagenPersonaje -> blit(pantallaDePrueba , 0 , 0);
						SDL_Flip(pantallaDePrueba);
						SDL_Delay( 100 );
						surfaceImagenPersonaje = imagen.getSurface();
					}
					imagen.setAccion(accion-volver);
					for (int i = 0 ; i < 12 ; i++) {
						SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
						surfaceImagenPersonaje -> blit(pantallaDePrueba , 0 , 0);
						SDL_Flip(pantallaDePrueba);
						SDL_Delay( 150 );
						surfaceImagenPersonaje = imagen.getSurface();
					}
				}
			}
		} else {
			SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_ROJO);
			retorno = false;
		}
		
		//Actualizar
		SDL_Flip(pantallaDePrueba);
	
		//tiempo de espera
		SDL_Delay( 200 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		
		return retorno;
	}