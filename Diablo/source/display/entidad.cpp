#include "entidad.h"
#include "tile.h"
#include "mapa.h"
//#include "../utilities/coordenadas.h"

	/*
		Pre:-

		Post: Inicializa los atributos con los valores por defecto.
	*/
	void Entidad::inicializarAtributosEnValoresDefault() {
		//posicion
		this -> offsetTileX = 0;
		this -> offsetTileY = 0;
		this -> posTileX = 0;
		this -> posTileY = 0;
		//tile destino
		this -> posTileDestinoX = this -> posTileX;
		this -> posTileDestinoY = this -> posTileDestinoY;
		//dimensiones
		this -> highInTiles = 1;
		this -> widthInTiles = 1;
		//pixel de referencia
		this -> pixel_ref_x = Entidad::PIXEL_REF_X_DEFAULT;
		this -> pixel_ref_y = Entidad::PIXEL_REF_Y_DEFAULT;
		//nombre
		this -> name = "";
		//surf e imagen
		this -> surf = NULL;
		this -> imagen = NULL;
		//velocidad
		this -> velocidad = Entidad::VELOCIDAD_DEFAULT;
	}

	/*
		Pre:-
		 
		Post: Se ha creado la instancia con los valores por defecto.
	*/
	Entidad::Entidad(){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		// Se setean los atributos a sus valores por defecto.
		this -> inicializarAtributosEnValoresDefault();
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenEstatica
	*/
	Entidad::Entidad(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles ,
					int pixel_ref_x ,int pixel_ref_y,
					unsigned int posTileX, unsigned int posTileY , 
					ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , pixel_ref_x , pixel_ref_y, posTileX, posTileY , rm , colorKey);
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad::Entidad(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , 
					const unsigned int fps , const unsigned int delay , 
					int pixel_ref_x ,int pixel_ref_y,
					unsigned int posTileX, unsigned int posTileY , 
					ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay ,
			Imagen::ALTO_DEFAULT , Imagen::ANCHO_DEFAULT ,
			Entidad::VELOCIDAD_DEFAULT ,
			pixel_ref_x , pixel_ref_y, 
			posTileX , posTileY , rm , colorKey);
	}
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad::Entidad(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			const unsigned int posTileX , const unsigned int posTileY , 
			ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay , 
			altoSprite , anchoSprite ,
			Entidad::VELOCIDAD_DEFAULT ,
			pixel_ref_x , pixel_ref_y, 
			posTileX , posTileY , rm , colorKey);
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad::Entidad(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned int anchoSprite ,
			const unsigned int velocidad , 
			const int pixel_ref_x , const int pixel_ref_y,
			const unsigned int posTileX , const unsigned int posTileY , 
			ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay , 
			altoSprite , anchoSprite ,
			velocidad,
			pixel_ref_x , pixel_ref_y, 
			posTileX , posTileY , rm , colorKey);
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	void Entidad::init(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , 
					const unsigned int fps , const unsigned int delay , 
					const unsigned int  altoSprite , const unsigned int anchoSprite ,
					const unsigned int velocidad ,
					int pixel_ref_x ,int pixel_ref_y,
					unsigned int posTileX, unsigned int posTileY , 
					ResMan& rm , const int colorKey) {
		// Se destruyen imagenes previas
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		// Se setean los atributos a sus valores por defecto.
		this -> inicializarAtributosEnValoresDefault();
		// Se carga la nueva imagen
		this -> imagen	= new ImagenAnimada(name.c_str() , altoSprite , 
								anchoSprite , fps , delay , rm ,colorKey);
		// Seteo de nombre 
		this -> name = name;
		// Seteo del Surface
		this -> surf = this -> imagen -> getSurface();
		//Seteo de dimensiones
		this -> highInTiles = hTiles;
		this -> widthInTiles = wTiles;
		//pixel de referencia
		this -> pixel_ref_x = pixel_ref_x ;
		this -> pixel_ref_y = pixel_ref_y;
		//posicion en Tiles
		this -> posTileX = posTileX;
		this -> posTileY = posTileY;
		//tile destino
		this -> posTileDestinoX = this -> posTileX;
		this -> posTileDestinoY = this -> posTileY;
		//seteo de velocidad
		this -> velocidad = velocidad;
	}

	/*
		Pre: La instancia ha sido creada.

		Post: Se ha configurado la instancia de acuerdo a a los parametros.

		NOTA: ImagenEstatica
	*/
	void Entidad::init(const std::string& name,  
					const unsigned int wTiles , const unsigned int hTiles , 
					int pixel_ref_x ,int pixel_ref_y , 
					unsigned int posTileX, unsigned int posTileY , 
					ResMan& rm , const int colorKey){
		// Se destruyen imagenes previas
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		// Se setean los atributos a sus valores por defecto.
		this -> inicializarAtributosEnValoresDefault();
		// Se carga la nueva imagen
		this -> imagen	= new ImagenEstatica( name.c_str() , rm , colorKey);
		// Seteo de nombre
		this -> name = name;
		// Seteo del Surface
		this -> surf = this -> imagen -> getSurface();
		//Seteo de dimensiones
		this -> highInTiles = hTiles;
		this -> widthInTiles = wTiles;
		//pixel de referencia
		this -> pixel_ref_x = pixel_ref_x ;
		this -> pixel_ref_y = pixel_ref_y;
		//posicion en Tiles
		this -> posTileX = posTileX;
		this -> posTileY = posTileY;
		//tile destino
		this -> posTileDestinoX = this -> posTileX;
		this -> posTileDestinoY = this -> posTileY;
	}
	
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	Entidad::~Entidad() {
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> surf = NULL;
	}

	/*
		Pre: Los parámetros respetan la siguiente convención:

			"x" e "y": Coordenadas del Tile destino

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
		
		Nota2: Los destinos no validos no traeran problemas al algoritmo, es decir que
		le podes pasar cualquier destino aunque supere las dimensiones del mapa.
	*/
	void Entidad::mover(const unsigned int x , const unsigned int y) {
		this -> posTileDestinoX = x;
		this -> posTileDestinoY = y;
	}
	
	/*
		Retorna la posición X en Tiles
	*/
	unsigned int Entidad::getPosTileX() const {
		return this -> posTileX;
	}
	
	/*
		Retorna la posición Y en Tiles
	*/
	unsigned int Entidad::getPosTileY() const {
		return this -> posTileY;
	}
			
	/*
		retorna el alto en tiles
	*/
	unsigned int Entidad::getHighInTiles() {
		return this -> highInTiles;
	}

	/*
		retorna el ancho en tiles
	*/
	unsigned int Entidad::getWidthInTiles() {
		return this -> widthInTiles;
	}

	// Actualiza las cosas internas, si hubiese
	void Entidad::update(Mapa* mapa) {
		//actualizacion de posicion
		this -> actualizarPosicion(mapa);
		// Actualizacion del surface
		this -> surf = this -> imagen -> getSurface();
	}
	
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
	void Entidad::blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
						const unsigned int tileX ,	const unsigned int tileY){
		if ( (this -> imagen != NULL) && (this -> surf != NULL) &&
			(camara != NULL) ) {
			if(this -> surf -> getSDL_Surface() != NULL){
				int posX = tileX - (int)(camara -> getX()) + this -> offsetTileX
					- this -> pixel_ref_x;
				int posY = tileY - (int)(camara -> getY()) + this -> offsetTileY
					- this -> pixel_ref_y;
				this -> surf -> blit(dest , posX ,posY);		
			}
		}
	}

	/*
		Pre: 

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
	unsigned int Entidad::calcularDireccion(){
		int deltaX = this -> posTileDestinoX - this -> posTileX;
		int deltaY = this -> posTileDestinoY - this -> posTileY;
		if (deltaX > 0){
			if(deltaY < 0){
				return Entidad::ESTE;
			} else{
				if(deltaY == 0){
					return Entidad::SURESTE;
				} else {
					return Entidad::OESTE;
				}
			}
		}else {
			if (deltaX < 0) {
				if (deltaY < 0){
					return Entidad::NORTE;
				}else{
					if (deltaY == 0) {
						return Entidad::NOROESTE;
					}else {
						return Entidad::OESTE;
					}
				}
			}else{
				if (deltaY < 0){
					return Entidad::NORESTE;
				} else{
					if(deltaY == 0){
						return Entidad::CENTRO;
					}else{
						return Entidad::SUROESTE;
					}
				}
			}
		}
	}

	void Entidad::actualizarPosicion(Mapa* mapa) {
		//Calculo de direccion
		unsigned int direccion = this -> calcularDireccion();
		/*
		printf("posDestX: %d posDestY: %d direccion: %u \n" , this->posTileDestinoX ,
			this->posTileDestinoY , direccion);*/
		/*
		// Si la direccion es centro se dirige al mismo
		if (direccion == Entidad::CENTRO) {
			
		//Si es otro Tile se actualizacion posiciones hacia el mismo
		} else {
			//Obtengo el tile actual
			Tile* tileActual = mapa -> getTile(this -> posTileX , this -> posTileY);
			if (tileActual != NULL) {
				// Calculo offset tentativo
				int offsetTentativoX = 0;
				int offsetTentativoY = 0;
				this -> calcularOffsetTentativo(direccion , &offsetTentativoX , 
															&offsetTentativoY);
				// Calculo posicion siguiente
				int posPixelSiguienteX = offsetTentativoX + tileActual -> getX() + this -> pixel_ref_x;
				int posPixelSiguienteY = offsetTentativoY + tileActual -> getY() + this -> pixel_ref_y;
				// Obtengo Tile origen
				Tile* tileOrigen = mapa -> getTile(0,0);
				// Obtengo el tile siguiente
				coordenadas coordenadas(Tile::TILE_ALTO , Tile::TILE_ANCHO,
										tileOrigen->getX(),tileOrigen->getY());
				int* tileSiguienteXY = coordenadas.coordenadas_tile(posPixelSiguienteX,
																posPixelSiguienteY);
				if (tileSiguienteXY != NULL){
					Tile* tileSiguiente = mapa -> getTile(tileSiguienteXY[0] , 
															tileSiguienteXY[1]);
					if (tileSiguiente != NULL){
						//tileSiguiente distinto a tileActual
						if(tileSiguiente != tileActual) {
							// Actualizacion offset
							if (offsetTileX > 0){
								this -> offsetTileX = offsetTentativoX - (Tile::TILE_ANCHO/2); 
							} else{
								this -> offsetTileX = (Tile::TILE_ANCHO/2) + offsetTentativoX; 
							}
							if (offsetTileY > 0){
								this -> offsetTileY = offsetTentativoY - (Tile::TILE_ALTO/2);
							} else{
								this -> offsetTileY = (Tile::TILE_ALTO/2) + offsetTentativoY;
							}
							// Remocion del Tile

							// Colocacion en el tile siguiente

							// Seteo de Entidad en nuevo Tile
							this -> posTileX = tileSiguienteXY[0];
							this -> posTileY = tileSiguienteXY[1];
							
						// tileSiguiente igual a tileActual
						} else {
							//actualizar offset
							this -> offsetTileX = offsetTentativoX;
							this -> offsetTileY = offsetTentativoY;
						}
					}
				}
			}
		}*/
	}

	void Entidad::calcularOffsetTentativo(unsigned int direccion , 
									int* offsetTentativoX , int* offsetTentativoY){
		*offsetTentativoX = this -> offsetTileX;
		*offsetTentativoY = this -> offsetTileY;
		switch (direccion){
				case NORTE :  {
					(*offsetTentativoX)-=this -> velocidad;
					(*offsetTentativoY)-=this -> velocidad;
					break;
				}
				case NORESTE : {
					(*offsetTentativoY)-=this -> velocidad;
					break;
				}
				case ESTE : {
					(*offsetTentativoX)+=this -> velocidad;
					(*offsetTentativoY)-=this -> velocidad;
					break;
				}
				case SURESTE : {
					(*offsetTentativoX)+=this -> velocidad;
					break;
				}
				case SUR : {
					(*offsetTentativoX)+=this -> velocidad;
					(*offsetTentativoY)+=this -> velocidad;
					break;
				}
				case SUROESTE : {
					(*offsetTentativoY)+=this -> velocidad;
					break;
				}
				case OESTE : {
					(*offsetTentativoX)-=this -> velocidad;
					(*offsetTentativoY)+=this -> velocidad;
					break;
				}
				case NOROESTE : {
					(*offsetTentativoX)-=this -> velocidad;
					break;
				}
		}
	}