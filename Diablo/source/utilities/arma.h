#pragma	once

#include "../../source/utilities/item.h"
#include "../../source/utilities/ImagenArma.h"

class Personaje;
class Item;
class Mapa;
class Tile;
class Arma: public Item {

public:
	
	//Pixeles de referencia default
	const static int PIXEL_REF_SPRITES_PRIMARIO_X = 0;
	const static int PIXEL_REF_SPRITES_PRIMARIO_Y = 70;
	const static int PIXEL_REF_SPRITES_ACCION_ESPECIAL_X = 20;
	const static int PIXEL_REF_SPRITES_ACCION_ESPECIAL_Y = 20;

	//Otras constantes default
	const static char DANIO_MAXIMO_DEFAULT = 20;
	
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
	
	//Informacion de acciones
	const static unsigned int MOVER_EN_CURSO = 0;
	const static unsigned int MOVER_COMPLETADO = 1;
	const static unsigned int MOVER_ERROR = 2;
	
	//Atacar
	const static unsigned int ATACAR_EN_CURSO = 3;
	const static unsigned int ATACAR_COMPLETADO = 4;
	const static unsigned int ATACAR_ERROR = 5;
		
	//Esperando accion
	const static unsigned int ESPERANDO_ACCION = 12;

	//Velocidad por defecto
	const static unsigned int VELOCIDAD_DEFAULT = 105; // En pixeles por segundo
		
	const static int MARGEN_ANCLA_X = 10;

	const static int MARGEN_ANCLA_Y = 10;

protected:

	Tile* tileDestino; // Tile destino

	double velocidad; // Pixeles por clock del procesador local

	clock_t deltaUpdatePosicion; // Distancia en ciclos del procesador local entre cada actualizacion

	clock_t tiempoProximoUpdate; // Tiempo proximo update en ciclos del procesador local

	bool actualizandoPosicion;

	bool cambioDireccionHabilitado;
	
protected:
	
	Personaje* propietario;
	
	char danio_maximo; // energia que quita el arma

public:

	//Constructor
	Arma(const std::string& name, 
		const unsigned int fps , const unsigned int delay ,
		const unsigned int velocidad ,
		const int pixel_ref_sprites_primario_x , const int pixel_ref_sprites_primario_y,
		Tile* tile, 
		ResMan& rm , const int colorKey , 
		char danio_maximo , Personaje* propietario);

	//Toma el item
	virtual void chocarCon(Personaje* personaje);

	unsigned int getDanioMaximo();

	Personaje* getPropietario();

	unsigned int calcularDeltaUptdatePosicionAdecuado(double velocidad);

	/*
		Pre: La instancia ha sido creada.
	 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	~Arma();
	
	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenArma
	*/
	virtual void init(const std::string& name,
					const unsigned int fps , const unsigned int delay ,
					const unsigned int velocidad ,
					const int pixel_ref_sprites_primario_x , const int pixel_ref_sprites_primario_y,
					Tile* tile, 
					ResMan& rm , const int colorKey , char danio_maximo , Personaje* propietario);
	
	/*
		Pre: Mapa distinto de null

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
	*/
	virtual void mover(Tile* tileDestino , Mapa* mapa);

	/*
		Pre: Mapa distinto de null

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
	*/
	virtual void mover(Tile* tileDestino);
	
	/*
		Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
		direcci�n del ataque.

		Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

	*/
	virtual unsigned int atacar(Mapa* mapa, Tile* tileDestino,Personaje* personaje, bool soloAnimacion);		 

	void accionEspecial();

	/*
		Pre: La instancia ha sido creada.
		Post: Se setea el Tile destino.
	*/
	virtual void setTileDestino(Tile* tile);

	virtual void setTileActual(Tile* tile);

	virtual void setTileActual(Tile* tile , Mapa* mapa);

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna el tile donde se encuentra la instancia.
	*/
	virtual Tile* getPosicion(Mapa* mapa);

	/*
		Pre: Se ha creado la instancia.

		Post: Se actualiza el movimiento de la instancia.
	*/
	virtual unsigned int update(Mapa* mapa);

	// Retorna el ancla de niebla X adecuada
	virtual int getXAnclajeNiebla();

	// Retorna el ancla de niebla Y adecuada
	virtual int getYAnclajeNiebla();
	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile donde se encuentra la
		instancia.
	*/
	virtual bool isCaminable(Tile* tile);

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile donde se encuentra la
		instancia.
	*/
	// Deprecated
	virtual bool isCaminable();	

	/*
		Pre: Los par�metros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el que se trata de dibujar la entidad.
			NOTA: Cuidado al momento de hacer updates, ya que hay entidades que 
			ocupan varios Tiles. En sintesis, un update por entidad al momento
			de pintar toda la pantalla.

		Post: Se ha pintado la entidad en el surface dest seg�n la camara y el mapa.

	*/
	virtual void blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY);

	/*
		Pre: Los par�metros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el que se trata de dibujar la entidad.
			NOTA: Cuidado al momento de hacer updates, ya que hay entidades que 
			ocupan varios Tiles. En sintesis, un update por entidad al momento
			de pintar toda la pantalla.

		Post: Se ha pintado la entidad en el surface dest seg�n la camara y el mapa.

	*/
	virtual void blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY , bool color);

protected:

	/*
		Pre: Los parametros definen el Tile destino.

		Post: Se retorna la direcci�n a la que se debe mover la entidad de acuerdo a la
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
	virtual unsigned int calcularDireccion(Mapa* mapa);


	virtual unsigned int actualizarPosicion(Mapa* mapa);

	
	virtual void calcularPosicionTentativa(unsigned int direccion , 
									int* offsetTentativoX , int* offsetTentativoY);
		
	virtual void actualizarImagen(const unsigned int direccion);

	virtual Tile* obtenerTileAncla(const int posX , const int posY , 
								const unsigned int direccion , Mapa* mapa);

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

 		Post: Se retorna verdadero si se puede ocupar el tile destino de la instancia, caso contrario
		se retorna falso.
	*/
	bool verificarDestinoCaminable(Mapa* mapa);

	bool verificarAncla(Tile* ancla);

	//Sin tener en cuenta al escudo
	void da�arPersonaje(Personaje* personajeObjetivo);
	
};