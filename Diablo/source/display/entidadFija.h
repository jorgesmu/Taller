#pragma	once

#include "../display/entidad.h"
#include "../utilities/ImagenPersonaje.h"
#include "../display/tile.h"
class Mapa;    // Declaración adelantada de Mapa
class Tile;    // Declaración adelantada de Tile
/*	
	Aclaraciones sobre esta clase:

	1) Puede tener imagenes animadas o estaticas.

	2) No esta compartida por varias posiciones, por lo tanto no se puede usar como
	pasto, agua, etc. Existirá una clase derivada que si permite esto.

	3) Puede tener una base dada por uno o más tiles.

	4) Se pinta en la posicion del ancla.

	* Acerca de Update:
	  -----------------
		
	
	* Acerca de blit:
	  ---------------
	Pinta en la posicion del ancla, con alguna clase de evaluacion acerca de si el ancla
	entra o no en la pantalla dada por la camara.

*/
class EntidadFija : public Entidad {
	
	public:
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	EntidadFija();
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los parametros.

		NOTA: ImagenEstatica
	*/
	EntidadFija(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey);
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	EntidadFija(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, Mapa* mapa,
			ResMan& rm , const int colorKey);

	
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	virtual ~EntidadFija();

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	virtual void init(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int pixel_ref_x , const int pixel_ref_y , 
					Tile* tile , Mapa* mapa,
					ResMan& rm , const int colorKey);
					
	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name,
					const unsigned int wTiles , const unsigned int hTiles , 
					const unsigned int fps , const unsigned int delay ,
					const unsigned int altoSprite , const unsigned int anchoSprite ,
					const int pixel_ref_x , const int pixel_ref_y,
					Tile* tile , Mapa* mapa,
					ResMan& rm , const int colorKey);
				
	/*
		retorna el alto en tiles
	*/
	unsigned int getHighInTiles();

	/*
		retorna el ancho en tiles
	*/
	unsigned int getWidthInTiles();

// -------------------------------------------------------------------------------------

	/*
		Va a ser borrada

	*/
	virtual void update(Mapa* mapa);
	
	/*
		
		Va a ser borrada.

	*/
	virtual void mover(Tile* tileDestino);

// -------------------------------------------------------------------------------------

	/*
		Pre: Los parámetros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el 

			NOTA: Cuidado al momento de hacer updates, ya que hay entidades que 
			ocupan varios Tiles. En sintesis, un update por entidad al momento
			de pintar toda la pantalla.

		Post: Se ha pintado la entidad en el surface dest según la camara y el mapa.
		Si la entidad tiene una base rectangular de un sólo Tile se pinta sin mayores 
		cuidados.
		En cambio si la entidad tiene una base superior a un tile se realiza un tratamiento
		especial.

	*/
	virtual void blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY);
	
	virtual void setTileActual(Tile* tile);

protected:	
	
	/*
		Obtiene el ancla adecuada
	*/
	virtual void actualizarTileAncla(Tile* tile, Mapa* mapa);

	
	virtual Tile* obtenerTileAncla(Tile* tile , Mapa* mapa);
};