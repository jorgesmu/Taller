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

	4) Se pinta en la posicion de las anclas.

	* Acerca de blit:
	  ---------------
	Pinta en la posicion de las anclas.


	NOTA:  La entidad Fija no es caminable.


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
			const unsigned int wTiles , const unsigned int hTiles , bool caminable,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey);
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	EntidadFija(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , bool caminable,
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, Mapa* mapa,
			ResMan& rm , const int colorKey);

	

	EntidadFija(EntidadFija* entidadFija , Mapa* mapa);
		
		
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	virtual ~EntidadFija();
	
	// Deprecated
	virtual void setColor(bool value);

	virtual void setColor(bool value , int tileX , int tileY, Mapa* mapa, int persX, int persY);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	virtual void init(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , bool caminable,
					const int pixel_ref_x , const int pixel_ref_y , 
					Tile* tile , Mapa* mapa,
					ResMan& rm , const int colorKey);
					
	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& name,
					const unsigned int wTiles , const unsigned int hTiles , bool caminable,
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
	
	/*
		Actualiza el surface de la instancia.

	*/
	virtual unsigned int update(Mapa* mapa);

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
					const unsigned int tileX ,	const unsigned int tileY , bool color);

	//deprecated
	virtual void setTileActual(Tile* tile);

	/*
		Pre: La instancia ha sido creada.

		Post: Si la instancia no tiene definida sus anclas, setea la posicion y las anclas.
	*/
	virtual void setTileActual(Tile* tile , Mapa* mapa);

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile ocupado 
		por dicha instancia.
	*/
	virtual bool isCaminable(Tile* tile);

	virtual bool isCaminable();
	
	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna el tile donde se encuentra la instancia.
	*/
	virtual Tile* getPosicion(Mapa* mapa);

	virtual void setDibujada(bool seDibujo, Mapa* mapa , Personaje* personaje);

	//Choca con un personaje
	virtual void chocarConPersonaje(Personaje* personaje);
	
protected:	
	
	/*
		Pre: La instancia ha sido creada.
		Post: Se han agregado las anclas correspondientes de acuerdo a la base.
	*/
	void agregarAnclas(Mapa* mapa);
};