#pragma	once

#include <string>
#include "../utilities/surface.h"
#include "../utilities/Imagen.h"
#include "../utilities/ImagenEstatica.h"
#include "../utilities/ImagenAnimada.h"
#include "SDL.h"

class Entidad {

	private:
		
		std::string name; // Nombre de la entidad
		
		std::string path; // Path a la imagen
		
		int x, y; // Posicion en la grilla
		
		Surface* surf; // Puntero a la surface de esta entidad (traida desde el resource manager on load)
		
		Imagen* imagen;

		int w , h; // Ancho y alto de la base en la grilla

		

	public:
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos x, y, w y h en cero, surf e 
		imagen en NULL, y name y path como cadenas vac?s.
	*/
	Entidad();

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	Entidad(const std::string& name, const std::string& path);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	Entidad(const std::string& name , const std::string& path , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	virtual void init(const std::string& name , const std::string& path);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	virtual void init(const std::string& name , const std::string& path , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, const std::string& path , const unsigned int w , 
				const unsigned int h , const int fps , const int delay);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, const std::string& path , const unsigned int w , 
				const unsigned int h , const int fps , const int delay , const int colorKey);

	/*
		Pre:

		Post:

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name, const std::string& path , const unsigned int w , 
				const unsigned int h , const int fps , const int delay);
	
	/*
		Pre:

		Post:

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name, const std::string& path , const unsigned int w , 
				const unsigned int h , const int fps , const int delay , const int colorKey);
		
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	virtual ~Entidad();
		
	// Cambia la posicion de la entidad
	virtual void mover(int x, int y);
	
	// Getters para la posicion
	int getX() const;
	
	int getY() const;
	// Actualiza las cosas internas, si hubiese
	
	virtual void update();
	
	// Dibuja la entidad
	virtual void blit(SDL_Surface* dest, SDL_Rect& cam);
	
	int getH();

	int getW();
		
};