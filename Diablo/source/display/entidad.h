#pragma	once

#include <string>
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

	* Acerca de mover:
	  ----------------
	El movimiento de la entidad se fija con la función "mover" de manera tal que una vez 
	especificado un "Tile Destino" la entidad fijara su curso hacia la misma y a medida que se 
	pida la actualización de su estado mediante "update", la misma actualizara su posición
	navegando por el mapa.
	Un llamado al método mover puede implicar que la instancia cambie su posicion dentro del Tile
	tomado como referencia, o que cambie de Tile.

	* Acerca de Update:
	  -----------------
	Update se encarga de actualizar posiciones sobre el Tile y sobre el Mapa a la velocidad de la
	entidad.

	Varios llamdos a Update sin hacer un blit, puede implicar que se produzcan varios 
	desplazamientos unitarios a la velocidad dada (algo así como pasos), de manera tal
	que se produzca un desplazamiento poco natural sobre el mapa.

	--> !!!! Importante: 
			Hacer solo un update por entidad al momento de pintar. Esto es porque
		si vas a pintar todos los tiles y haces un update por cada Tile pintado, puede pasar que
		se te mueva la entidad y te aparezca cortada en varios Tiles.
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	* Acerca de blit:
	  ---------------
	Como una entidad puede ocupar uno o varios Tiles, y la aplicacion actualiza la pantalla de
	acuerdo a un orden dado por los Tiles, al momento de llamar a blit se tendra que especificar
	sobre que "Tile" pintara la entidad.

*/
class Entidad {

	public:

		//Direccion
		const static unsigned int SUR = 0;
		const static unsigned int SURESTE = 1;
		const static unsigned int ESTE = 2;
		const static unsigned int NORESTE = 3; 
		const static unsigned int NORTE = 4;
		const static unsigned int NOROESTE = 5;
		const static unsigned int OESTE = 6;
		const static unsigned int SUROESTE = 7;	
		const static unsigned int CENTRO = 8;

		//Velocidad por defecto
		const static int VELOCIDAD_DEFAULT = 5;

		const static int DELTA_AVANCE = 2;

		//Base de tiempo para la velocidad
		const static int BASE_DE_TIEMPO = 1000;

		const static int BASE_DE_TIEMPO_RAPIDO = 300;

		const static int COTA_VELOCIDAD_BAJA = 50000;

		//Pixel de referencia por defecto
		const static int PIXEL_REF_X_DEFAULT = 0;

		const static int PIXEL_REF_Y_DEFAULT = 0;

		const static int MARGEN_ANCLA_X = 10;

		const static int MARGEN_ANCLA_Y = 10;

	protected:
		
		std::string name; // Nombre de la entidad
			
		Tile* tileDestino; // Tile destino

		Tile* tileAncla; // Tile donde se encuentra el ancla

		Surface* surf; // Puntero a la surface de esta entidad (traida desde el resource manager on load)
		
		Imagen* imagen; // Instancia de la clase imagen que contiene los sprites y la lógica de
						// animación de los sprites en caso que correspondiese
		unsigned int widthInTiles , highInTiles; // Ancho y alto de la base en la grilla [en tiles]

		int pixel_ref_x , pixel_ref_y; // Pixel de referencia dentro del espacio de la imagen
										//de la entidad.
		unsigned int deltaUpdatePosicion; // Distancia en tiempo, entre cada actualizacion
												//de posicion.
		unsigned int velocidad;

		clock_t tiempoProximoUpdate;

		int posX,  posY; //posicion en pixeles

		bool compartido; //si es compartido por varios tiles

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
			const unsigned int wTiles , const unsigned int hTiles ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , 
			ResMan& rm , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
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
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, 
			ResMan& rm , const int colorKey);

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const unsigned int velocidad ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , 
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
					const unsigned int wTiles , const unsigned int hTiles , 
					const int pixel_ref_x , const int pixel_ref_y , 
					Tile* tile , 
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
					const unsigned int velocidad ,
					const int pixel_ref_x , const int pixel_ref_y,
					Tile* tile , 
					ResMan& rm , const int colorKey);

	/*
		Pre: Los parámetros respetan la siguiente convención:

			"x" e "y": Coordenadas del Tile destino

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
	*/
	virtual void mover(Tile* tileDestino);
			
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
	
	void setTileActual(Tile* tile);

	void setTileDestino(Tile* tile);

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

	/*
		Pre: Los parametros definen el Tile destino.

		Post: Se retorna la dirección a la que se debe mover la entidad de acuerdo a la
		siguiente codificacion:

			SUR = 0;
			SURESTE = 1;
			ESTE = 2;
			NORESTE = 3; 
			NORTE = 4;
			NOROESTE = 5;
			OESTE = 6;
			SUROESTE = 7;	
			CENTRO = 8;
			
		NOTA: CENTRO implica que la entidad esta en el Tile destino y por lo tanto debe volver 
		aproximadamente al centro del mismo.
	*/
	unsigned int calcularDireccion(Mapa* mapa);

	void actualizarPosicion(Mapa* mapa);

	void calcularPosicionTentativa(unsigned int direccion , 
									int* offsetTentativoX , int* offsetTentativoY);

	virtual void actualizarImagen(const unsigned int direccion);

	Tile* obtenerTileAncla(const int posX , const int posY , 
								const unsigned int direccion , Mapa* mapa);

};