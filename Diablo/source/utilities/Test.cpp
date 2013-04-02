#include "Test.h"
#include "../display/entidad.h"
#include "../display/camara.h"
#include "../display/resman.h"
#include "../utilities/Personaje.h"

	//Todos los tests
	bool Test::test() {
		bool retorno = true;
		//retorno &= testSurface();
		//retorno &= testImagenEstatica();
		//retorno &= Test::testImagenAnimada();
		//retorno &= Test::testImagenPersonaje();
		//retorno &= Test::testEntidad();
		//retorno &= Test::testImagenEstaticaConResourceManager();
		//retorno &= Test::testImagenAnimadaConResourceManager();
		//retorno &= Test::testImagenPersonajeConResourceManager();
		//retorno &= Test::testPersonaje();
		return retorno;
	}
	
	//detectar salida
	bool salida() {	
		bool retorno = false;
		SDL_Event event;
		SDL_PollEvent(&event);
		// Detectar escape o quit
		if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) {
			retorno = true;
		}
		return retorno;
	}

	//Test Surface
	bool Test::testSurface() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
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
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//imagen estática a cargar
		ImagenEstatica imagen("../resources/arbol.bmp" , Surface::BMP_TRANSPARENCIA);
		//retorno del test
		bool retorno = true;
		//primera evaluación de imagen cargada correctamente
		retorno &= (imagen.getSurface() != NULL);
		Surface* imagenEstatica = imagen.getSurface();
		//Si imagen distinto de nulo
		if (imagenEstatica -> getSDL_Surface() != NULL) {
			for (int i = 0 ; i < 10; i++) {
				imagenEstatica -> blit(pantallaDePrueba , 0 , 0);
				imagenEstatica = imagen.getSurface();
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 200 );
				if (salida()) exit(0);
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
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//imagen animada a cargar
		ImagenAnimada imagen("../resources/Soldado.bmp" , 100 , 100 , 10 , 500 
								, Surface::BMP_TRANSPARENCIA);
				
		//primera evaluación de imagen cargada correctamente
		
		Surface* surfaceImagenAnimada = imagen.getSurface();
		retorno &= (surfaceImagenAnimada != NULL);
		//Si imagen distinto de nulo
		if (surfaceImagenAnimada != NULL) {
			for (int i = 0 ; i < 300; i++) {
				SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
				surfaceImagenAnimada -> blit(pantallaDePrueba , 100 , 100);
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 100 );
				surfaceImagenAnimada = imagen.getSurface();
				if (salida()) exit(0);
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
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//imagen animada a cargar
		ImagenPersonaje imagen("../resources/Soldado.bmp" , 100 , 100 , 10 , 500 
								, Surface::BMP_TRANSPARENCIA);
				
		//primera evaluación de imagen cargada correctamente
		
		Surface* surfaceImagenPersonaje = imagen.getSurface();
		retorno &= (surfaceImagenPersonaje != NULL);
		//Si imagen distinto de nulo
		if (surfaceImagenPersonaje != NULL) {
			
			for (unsigned int accion = ImagenPersonaje::DES_SUR ; (accion <= ImagenPersonaje::MUERTE)&&(!salida()) ; accion++) {
				for(int volver = 1 ; volver < (int) accion ; volver++) {
					imagen.setAccion(accion);
					for (int i = 0 ;i < 12; i++) {
						SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
						surfaceImagenPersonaje -> blit(pantallaDePrueba , 0 , 0);
						SDL_Flip(pantallaDePrueba);
						SDL_Delay( 100 );
						surfaceImagenPersonaje = imagen.getSurface();
						if (salida()) exit(0);
					}
					imagen.setAccion(accion-volver);
					for (int i = 0 ; i < 12 ; i++) {
						SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
						surfaceImagenPersonaje -> blit(pantallaDePrueba , 0 , 0);
						SDL_Flip(pantallaDePrueba);
						SDL_Delay( 150 );
						surfaceImagenPersonaje = imagen.getSurface();
						if (salida()) exit(0);
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

	//Test Entidad
	bool Test::testEntidad() {
		
		//retorno del test
		bool retorno = true;
		
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//camara
		Camara camara;
		camara.init(800,600,50);
		//Resman
		ResMan resman;		
		//imagen estatica a cargar
		
		Entidad entidadArbol("arbol" , "../resources/Arbol.bmp", 
							1 , 1 , 
							0 , 0 , 
							0 , 0, resman , Imagen::COLOR_KEY);
		//imagen animada
		Entidad entidadSoldado("soldado" , "../resources/Soldado.bmp", 
							0 , 0 , 
							10 , 500,
							0 , 0 , 
							0 , 1 , resman ,
							Imagen::COLOR_KEY);
		int contador = 1;
		while((!salida()) && (contador <5)) {
			entidadArbol.blit(pantallaDePrueba , camara , 100 , 100); 
			entidadSoldado.blit(pantallaDePrueba , camara , 100 , 100); 
			SDL_Flip(pantallaDePrueba);
			entidadArbol.mover( (contador) , 0  , 100 , 100);
			entidadSoldado.mover( contador , 1 , 100 , 100);	
			entidadSoldado.update();
			contador++;
			SDL_Delay( 100 );
		}
		while(!salida()) {
			SDL_Delay( 100 );
		}
		resman.clean();
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		return retorno;
	}

	//Test Imagen Estatica con Resource Manager
	bool Test::testImagenEstaticaConResourceManager() {
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//resource manager
		ResMan resman;
		resman.addRes("arbol", "../resources/arbol.bmp", Surface::BMP_TRANSPARENCIA);
		resman.addRes("arbol2", "../resources/arbol.bmp", Surface::BMP_TRANSPARENCIA);
		//imagen estática a cargar
		ImagenEstatica imagen("../resources/arbol.bmp" , "arbol" , resman);
		ImagenEstatica imagen2("../resources/arbol.bmp" , "arbol3" , resman , Imagen::COLOR_KEY);
		//retorno del test
		bool retorno = true;
		//primera evaluación de imagen cargada correctamente
		retorno &= (imagen.getSurface() != NULL);
		Surface* imagenEstatica = imagen.getSurface();
		//Si imagen distinto de nulo
		if (imagenEstatica -> getSDL_Surface() != NULL) {
				imagenEstatica -> blit(pantallaDePrueba , 0 , 0);
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 200 );
		} else {
			retorno = false;
		}

		imagenEstatica = imagen2.getSurface();
		
		//Si imagen distinto de nulo
		if (imagenEstatica != NULL){
			if(imagenEstatica -> getSDL_Surface() != NULL) {
				imagenEstatica -> blit(pantallaDePrueba , 200 , 200);
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 200 );
			} else {
				retorno = false;
			}
		} else {
			retorno = false;
		}
		
		//tiempo de espera
		SDL_Delay( 100 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		
		//liberar resman
		resman.clean();

		return retorno;
	}

	//Test Imagen Animada
	bool Test::testImagenAnimadaConResourceManager() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//resman

		ResMan resman;

		//imagen animada a cargar
		ImagenAnimada imagen("../resources/Soldado.bmp" ,"Soldado" ,100 , 100 , 10 , 500 
								, resman , Surface::BMP_TRANSPARENCIA);
				
		//primera evaluación de imagen cargada correctamente
		
		Surface* surfaceImagenAnimada = imagen.getSurface();
		retorno &= (surfaceImagenAnimada != NULL);
		//Si imagen distinto de nulo
		if (surfaceImagenAnimada != NULL) {
			for (int i = 0 ; i < 300 ; i++) {
				SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
				surfaceImagenAnimada -> blit(pantallaDePrueba , 100 , 100);
				SDL_Flip(pantallaDePrueba);
				SDL_Delay( 100 );
				surfaceImagenAnimada = imagen.getSurface();
				if (salida()) exit(0);
			}

		} else {
			SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_ROJO);
			retorno = false;
		}
		//liberar resman
		resman.clean();
	
		//tiempo de espera
		SDL_Delay( 200 );
	
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		
		return retorno;
	}

	//Test Imagen Personaje
	bool Test::testImagenPersonajeConResourceManager() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//resman
		ResMan resman;

		//imagen animada a cargar
		ImagenPersonaje imagen("../resources/Soldado.bmp" , "Soldado" , 100 , 100 , 10 , 500 
								, resman, Surface::BMP_TRANSPARENCIA);
				
		//primera evaluación de imagen cargada correctamente
		
		Surface* surfaceImagenPersonaje = imagen.getSurface();
		retorno &= (surfaceImagenPersonaje != NULL);
		//Si imagen distinto de nulo
		if (surfaceImagenPersonaje != NULL) {
			
			for (unsigned int accion = ImagenPersonaje::DES_SUR ; (accion <= ImagenPersonaje::MUERTE)&&(!salida()) ; accion++) {
				for(int volver = 1 ; volver < (int) accion ; volver++) {
					imagen.setAccion(accion);
					for (int i = 0 ; i < 12 ; i++) {
						SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
						surfaceImagenPersonaje -> blit(pantallaDePrueba , 0 , 0);
						SDL_Flip(pantallaDePrueba);
						SDL_Delay( 100 );
						surfaceImagenPersonaje = imagen.getSurface();
						if (salida()) exit(0);
					}
					imagen.setAccion(accion-volver);
					if (salida()) exit(0);
					for (int i = 0 ; i < 12 ; i++) {
						SDL_FillRect(pantallaDePrueba , NULL , Surface::RGB_VERDE);
						surfaceImagenPersonaje -> blit(pantallaDePrueba , 0 , 0);
						SDL_Flip(pantallaDePrueba);
						SDL_Delay( 150 );
						surfaceImagenPersonaje = imagen.getSurface();
						if (salida()) exit(0);
					}
					if (salida()) exit(0);
				}
				if (salida()) exit(0);
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
		
		//liberacion de resman
		resman.clean();

		return retorno;
	}

	//Test Personaje
	bool Test::testPersonaje() {
		//retorno del test
		bool retorno = true;
		//pantalla de prueba
		SDL_Surface* pantallaDePrueba = SDL_SetVideoMode( 800, 600 , 32, SDL_DOUBLEBUF);
		SDL_WM_SetCaption( "Presione ESC para salir", NULL );
		//camara
		Camara camara;
		camara.init(800,600,50);
		//Resman
		ResMan resman;		
		//Personaje
		Personaje personaje("soldado" , "../resources/Soldado.bmp", 
							0 , 0 , 
							10 , 500, 50,
							0 , 0 , 
							0 , 1 , resman ,
							Imagen::COLOR_KEY);
		int contador = 1;
		while((!salida()) && (contador <8)) {
			personaje.blit(pantallaDePrueba , camara , 100 , 100); 
			SDL_Flip(pantallaDePrueba);
			personaje.mover( contador , 1 , 100 , 100);	
			personaje.update();
			contador++;
			SDL_Delay( 100 );
		}
		while(!salida()) {
			SDL_Delay( 100 );
		}
		resman.clean();
		//Destruir principal
		SDL_FreeSurface(pantallaDePrueba);
		return retorno;
	}