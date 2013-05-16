#pragma	once
#include <string>
#include <vector>
#include "../utilities/surface.h"
#include "../utilities/Imagen.h"
#include "../utilities/ImagenEstatica.h"
#include "../utilities/ImagenAnimada.h"
#include "../display/camara.h"
#include "../display/resman.h"
#include "SDL.h"
class Mapa;    // Declaración adelantada de Mapa
class Tile;    // Declaración adelantada de Tile
/*	
	Aclaraciones sobre esta clase:
	
	* Una instancia de esta clase es caminable por defecto, pero las clases derivadas de ella
	pueden no serlo.

	* Acerca de Update:
	  -----------------
	Update se encarga de actualizar el estado de la entidad.

*/
class Entidad {

	public:
		
		//Pixel de referencia por defecto
		const static int PIXEL_REF_X_DEFAULT = 0;

		const static int PIXEL_REF_Y_DEFAULT = 0;

	protected:
		
		std::string name; // Nombre de la entidad
		
		Tile* tileAncla; // Tile donde se encuentra el ancla

		Surface* surf; // Puntero a la surface de esta entidad (traida desde el resource manager on load)
		
		Imagen* imagen; // Instancia de la clase imagen que contiene los sprites y la lógica de
						// animación de los sprites en caso que correspondiese
		unsigned int widthInTiles , highInTiles; // Ancho y alto de la base en la grilla [en tiles]

		int pixel_ref_x , pixel_ref_y; // Pixel de referencia dentro del espacio de la imagen
										//de la entidad.
	
		int posX,  posY; //posicion en pixeles

		bool compartido; //si es compartido por varios tiles

		bool dibujada; //

		std::vector<Tile*> tilesExplorados; //los tiles que ya visito

		bool color;

		bool caminable;

public:
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	Entidad();
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los parametros.

		NOTA: ImagenEstatica
	*/
	Entidad(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , 
			ResMan& rm , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
			const unsigned int fps , const unsigned int delay , 
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile,
			ResMan& rm , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, 
			ResMan& rm , const int colorKey);
	
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
	virtual void init(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
					const int pixel_ref_x , const int pixel_ref_y , 
					Tile* tile , 
					ResMan& rm , const int colorKey);
					
	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name,
					const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
					const unsigned int fps , const unsigned int delay ,
					const unsigned int altoSprite , const unsigned int anchoSprite ,
					const int pixel_ref_x , const int pixel_ref_y,
					Tile* tile , 
					ResMan& rm , const int colorKey);

	std::string get_nombre();
				
	/*
		retorna el alto en tiles
	*/
	unsigned int getHighInTiles();

	/*
		retorna el ancho en tiles
	*/
	unsigned int getWidthInTiles();

	/*
		Pre: Se ha creado la instancia.

		Post: Se actualiza el movimiento de la instancia.
	*/
	virtual void update(Mapa* mapa);
	
	/*
		Pre: Los parámetros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el que se trata de dibujar la entidad.
			NOTA: Cuidado al momento de hacer updates, ya que hay entidades que 
			ocupan varios Tiles. En sintesis, un update por entidad al momento
			de pintar toda la pantalla.

		Post: Se ha pintado la entidad en el surface dest según la camara y el mapa.

	*/
	virtual void blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY /*Tile* tile*/);
	
	/*
	virtual void blitGris(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY );
	*/
	//deprecated
	virtual void setTileActual(Tile* tile);
	
	
	virtual void setTileActual(Tile* tile , Mapa* mapa);

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile donde se encuentra la
		instancia.
	*/
	virtual bool isCaminable(Tile* tile , Mapa* mapa);

	virtual bool isCaminable();
	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna el tile donde se encuentra la instancia.
	*/
	virtual Tile* getPosicion(Mapa* mapa);

	unsigned int getX();

	unsigned int getY();

	bool getDibujada();

	void setDibujada(bool seDibujo);

	virtual std::vector<Tile*> getTilesExplorados();

	void agregarTilesExplorados(Tile* tile);

	virtual void setColor(bool value);

	virtual void setColor(bool value , int tileX , int tileY);

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
	virtual void inicializarAtributosEnValoresDefault();

	/*
		Pre: La instancia ha sido creada.

		Post: Se actualiza la imagen de la instancia.
	*/
	virtual void actualizarImagen(const unsigned int direccion);

};