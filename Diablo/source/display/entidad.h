#pragma	once

#include <string>
#include "../utilities/surface.h"
#include "../utilities/Imagen.h"
#include "../utilities/ImagenEstatica.h"
#include "../utilities/ImagenAnimada.h"
#include "../display/camara.h"
#include "SDL.h"

/*	
	Axiomas de Entidad:

	*	Entidad tiene la capacidad de moverse entre Tiles, de manera tal que para un movimiento se le
	tiene que informar el tamaño del Tile correspondiente en pixeles.
*/
class Entidad {

	public:

		const static int PIXEL_REF_X_DEFAULT = 0;

		const static int PIXEL_REF_Y_DEFAULT = 0;

	protected:
		
		std::string name; // Nombre de la entidad
		
		std::string path; // Path a la imagen
		
		int posTileX, posTileY; // Posicion en la grilla [en tiles]
		
		int offsetTileX, offsetTileY; // Posicion en el Tile

		Surface* surf; // Puntero a la surface de esta entidad (traida desde el resource manager on load)
		
		Imagen* imagen; // Instancia de la clase imagen que contiene los sprites y la lógica de
						// animación de los sprites en caso que correspondiese

		int widthInTiles , highInTiles; // Ancho y alto de la base en la grilla [en tiles]

		int pixel_ref_x , pixel_ref_y; // Pixel de referencia dentro del espacio de la imagen
										//de la entidad.

	public:
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos x, y, w y h en cero, surf e 
		imagen en NULL, y name y path como cadenas vac?s.
	*/
	Entidad();
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los parametros.

		NOTA: ImagenEstatica
	*/
	Entidad(const std::string& name, const std::string& path , 
			const unsigned int wTiles , const unsigned int hTiles ,
			const int pixel_ref_x , const int pixel_ref_y,
			const int posTileX , const int posTileY , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, const std::string& path , 
			const unsigned int wTiles , const unsigned int hTiles , 
			const int fps , const int delay , 
			const int pixel_ref_x , const int pixel_ref_y,
			const int posTileX , const int posTileY , const int colorKey);
	
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	virtual ~Entidad();

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	virtual void init(const std::string& name, const std::string& path , 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int pixel_ref_x , const int pixel_ref_y , 
					const int posTileX, const int posTileY , const int colorKey);
			
	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name, const std::string& path , 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int fps , const int delay , 
					const int pixel_ref_x , const int pixel_ref_y,
					const int posTileX , const int posTileY , const int colorKey);
			
	// Cambia la posicion de la entidad
	virtual void mover(const unsigned int x , const unsigned int y , 
					const unsigned int altoTileEnPixeles , 
					const unsigned int anchoTileEnPixeles);
	
	/*
		Retorna la posición X en Tiles
	*/
	int getPosTileX() const;
	
	/*
		Retorna la posición Y en Tiles
	*/
	int getPosTileY() const;

	/*
		retorna el alto en tiles
	*/
	int getHighInTiles();

	/*
		retorna el ancho en tiles
	*/
	int getWidthInTiles();
	
	// Actualiza las cosas internas, si hubiese
	
	virtual void update();
	
	// Dibuja la entidad
	virtual void blit(SDL_Surface* dest, const Camara& camara , 
					const unsigned int altoTileEnPixeles , 
					const unsigned int anchoTileEnPixeles);

protected:	
		
	/*
		Pre:-

		Post: Inicializa los atributos con los valores por defecto.

			name:  Cadena vacia.

			path: Cadena vacia.

			posTileX , posTileY: Ambos en cero.

			posPixelX , posPixelY: Ambos en cero.

			surf: Puntero nulo.

			imagen: Puntero nulo.

			widthInTiles , highInTiles: En 1.

			pixel_ref_x , pixel_ref_y: En los valores default dados por las constantes
										correspondientes.

	*/
	void inicializarAtributosEnValoresDefault();

};