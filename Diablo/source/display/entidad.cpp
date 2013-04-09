#include "entidad.h"
#include "tile.h"
#include "mapa.h"


	/*
		Pre:-

		Post: Inicializa los atributos con los valores por defecto.
	*/
	void Entidad::inicializarAtributosEnValoresDefault() {
		//posicion
		this -> offsetTileX = 0;
		this -> offsetTileY = 0;
		//tile actual
		this -> tileActual = NULL;
		//tile destino
		this -> tileDestino = NULL;
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
		//tiempo siguiente update
		this -> tiempoProximoUpdate = clock();
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
					Tile* tile,
					ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , pixel_ref_x , pixel_ref_y, tile , rm , colorKey);
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
					Tile* tile , 
					ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay ,
			Imagen::ALTO_DEFAULT , Imagen::ANCHO_DEFAULT ,
			Entidad::VELOCIDAD_DEFAULT ,
			pixel_ref_x , pixel_ref_y, 
			tile , rm , colorKey);
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
			Tile* tile , 
			ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay , 
			altoSprite , anchoSprite ,
			Entidad::VELOCIDAD_DEFAULT ,
			pixel_ref_x , pixel_ref_y, 
			tile, rm , colorKey);
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
			Tile* tile , 
			ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay , 
			altoSprite , anchoSprite ,
			velocidad,
			pixel_ref_x , pixel_ref_y, 
			tile , rm , colorKey);
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
					Tile* tile , 
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
		if (this -> tileActual != NULL) {
			this -> tileActual -> deleteEntidad(this);
		}
		this -> tileActual = tile;
		if (tile != NULL) {
			this -> tileActual -> addEntidad(this);
		}
		//tile destino
		this -> tileDestino = tileActual;
		//seteo de velocidad
		this -> velocidad = velocidad;
		//tiempo siguiente update
		this -> tiempoProximoUpdate = clock();
	}

	/*
		Pre: La instancia ha sido creada.

		Post: Se ha configurado la instancia de acuerdo a a los parametros.

		NOTA: ImagenEstatica
	*/
	void Entidad::init(const std::string& name,  
					const unsigned int wTiles , const unsigned int hTiles , 
					int pixel_ref_x ,int pixel_ref_y , 
					Tile* tile , 
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
		if (this -> tileActual != NULL) {
			this -> tileActual -> deleteEntidad(this);
		}
		this -> tileActual = tile;
		if (tile != NULL) {
			this -> tileActual -> addEntidad(this);
		}
		//tile destino
		this -> tileDestino = tileActual;
		//tiempo siguiente update
		this -> tiempoProximoUpdate = clock();
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
		this -> tileActual = NULL;
		this -> tileDestino = NULL;
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
	void Entidad::mover(Tile* tileDestino) {
		if (tileDestino != NULL){
			this -> tileDestino = tileDestino;
		}
	}
	
	/*
		Retorna Tile Actual
	*/
	Tile* Entidad::getTileActual(){
		return this -> tileActual;
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
		if (this -> tiempoProximoUpdate <= clock()){
			if (this->tileDestino != NULL) {
				//actualizacion de posicion
				this -> actualizarPosicion(mapa);
			} else {
				if(this->imagen != NULL) {
					this -> surf = this -> imagen -> getSurface();
				}
			}
			this ->tiempoProximoUpdate = clock() + Entidad::DELTA_UPDATE;
		} else {
			if(this -> imagen != NULL) {
				// Actualizacion del surface
				this -> surf = this -> imagen -> getSurface();
			}
		}
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
	void Entidad::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
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
		if ((tileDestino != NULL) && (tileActual != NULL)) {
			int deltaX = this -> tileDestino -> getX() - this -> tileActual -> getX();
			int deltaY = this -> tileDestino -> getY() - this -> tileActual -> getY();
			if (deltaX > 0){
				if(deltaY < 0){
					return Entidad::NORESTE;
				} else{
					if(deltaY == 0){
						return Entidad::ESTE;
					} else {
						return Entidad::SURESTE;
					}
				}
			}else {
				if (deltaX < 0) {
					if (deltaY < 0){
						return Entidad::NOROESTE;
					}else{
						if (deltaY == 0) {
							return Entidad::OESTE;
						}else {
							return Entidad::SUROESTE;
						}
					}
				}else{
					if (deltaY < 0){
						return Entidad::NORTE;
					} else{
						if(deltaY == 0){
							return Entidad::CENTRO;
						}else{
							return Entidad::SUR;
						}
					}
				}
			}
		}
	}

	void Entidad::actualizarPosicion(Mapa* mapa) {
		//Calculo de direccion
		unsigned int direccion = this -> calcularDireccion();
	/*	printf("Actual %d %d Destino %d %d Direccion %u\n", tileActual->getX() , tileActual->getY() ,
				tileDestino->getX() , tileDestino->getY() , direccion);*/
		// Si la direccion es centro se dirige al mismo
		if (direccion == Entidad::CENTRO) {
			this -> actualizarImagen(direccion);
		//Si es otro Tile se actualizacion posiciones hacia el mismo
		} else {
			if (tileActual != NULL) {
				// Calculo offset tentativo (sin tener en cuenta el pixel de referencia)
				int offsetTentativoX = 0;
				int offsetTentativoY = 0;
				this -> calcularOffsetTentativo(direccion , &offsetTentativoX , 
															&offsetTentativoY);
				// Calculo posicion siguiente (tomando en cuenta pixel de referencia y la posicion tile actual)
				int posPixelSiguienteX = offsetTentativoX + tileActual -> getX();
				int posPixelSiguienteY = offsetTentativoY + tileActual -> getY();
				// Obtengo el tile siguiente
				Tile* tileSiguiente = this->obtenerTileSiguiente(
												posPixelSiguienteX , posPixelSiguienteY , 
												direccion , mapa);
				// Si el tileSiguiente es no nulo continua, sino no hace nada
				if (tileSiguiente != NULL){
					//tileSiguiente distinto a tileActual
					if(tileSiguiente != tileActual) {
						// Actualizacion offset
						/*
						
						this -> offsetTileX = this -> tileActual -> getX() - 
										(tileSiguiente -> getX() + offsetTentativoX); 
						this -> offsetTileY = this -> tileActual -> getY() - 
										(tileSiguiente -> getY() + offsetTentativoY); 
						// Remocion del Tile
						this -> tileActual -> deleteEntidad(this);
						// Colocacion en el tile siguiente
						tileSiguiente -> addEntidad(this);
						// Seteo de Entidad en nuevo Tile
						this -> setTileActual(tileSiguiente);
						this ->actualizarImagen(direccion);*/
					// tileSiguiente igual a tileActual
					} else {
						//actualizar offset
						this -> offsetTileX = offsetTentativoX;
						this -> offsetTileY = offsetTentativoY;
					}
				}
			}
		}
	}
	/* 
		El offset X Y sin tener en cuenta el pixel de origen
	*/
	void Entidad::calcularOffsetTentativo(unsigned int direccion , 
									int* offsetTentativoX , int* offsetTentativoY){
		*offsetTentativoX = this -> offsetTileX;
		*offsetTentativoY = this -> offsetTileY;
		switch (direccion){
				case NORTE :  {
					(*offsetTentativoY)-=this -> velocidad;
					break;
				}
				case NORESTE : {
					(*offsetTentativoX)+=(2*this -> velocidad);
					(*offsetTentativoY)-=this -> velocidad;
					break;
				}
				case ESTE : {
					(*offsetTentativoX)+=(2*this -> velocidad);
					break;
				}
				case SURESTE : {
					(*offsetTentativoX)+=(2*this -> velocidad);
					(*offsetTentativoY)+=this -> velocidad;
					break;
				}
				case SUR : {
					(*offsetTentativoY)+=this -> velocidad;
					break;
				}
				case SUROESTE : {
					(*offsetTentativoX)-=(2*this -> velocidad);
					(*offsetTentativoY)+=this -> velocidad;
					break;
				}
				case OESTE : {
					(*offsetTentativoX)-=(2*this -> velocidad);
					break;
				}
				case NOROESTE : {
					(*offsetTentativoX)-=(2*this -> velocidad);
					(*offsetTentativoY)-=this -> velocidad;
					break;
				}
		}
	}

	void Entidad::actualizarImagen(const unsigned int direccion){
		if(this -> imagen != NULL) {
			// Actualizacion del surface
			this -> surf = this -> imagen -> getSurface();
		}
	}

	/*
		posX y posY en pixeles, es una posicion cualquiera en el mapa
	*/
	Tile* Entidad::obtenerTileSiguiente(const int posX , const int posY , 
													const unsigned int direccion , Mapa* mapa){
	Tile* retorno = mapa -> getTilePorPixeles(posX , posY);
	int posImagenX = posX + this -> imagen -> getAncho() - this -> pixel_ref_x;
	int posImagenY = posY + this -> imagen -> getAlto() - this -> pixel_ref_y;
	Tile* tilePosicionImagen = mapa -> getTile(posImagenX , posImagenY);
	if((retorno != NULL) && (tilePosicionImagen != NULL) && 
		(tilePosicionImagen != retorno)){
		int xSiguiente = this -> tileActual -> getX();
		int ySiguiente = this -> tileActual -> getY();
		switch (direccion){
			case NORTE :  {
				ySiguiente -= Tile::TILE_ALTO;
				break;
			}
			case NORESTE : {
				xSiguiente += Tile::TILE_ANCHO/2;
				ySiguiente -= Tile::TILE_ALTO/2;
				break;
			}
			case ESTE : {
				xSiguiente += Tile::TILE_ANCHO;
				break;
			}
			case SURESTE : {
				xSiguiente += Tile::TILE_ANCHO/2;
				ySiguiente += Tile::TILE_ALTO/2;
				break;
			}
			case SUR : {
				ySiguiente += Tile::TILE_ALTO;
				break;
			}
			case SUROESTE : {
				xSiguiente -= Tile::TILE_ANCHO/2;
				ySiguiente += Tile::TILE_ALTO/2;
				break;
			}
			case OESTE : {
				xSiguiente -= Tile::TILE_ANCHO;
				break;
			}
			case NOROESTE : {
				xSiguiente -= Tile::TILE_ANCHO/2;
				ySiguiente -= Tile::TILE_ALTO/2;
				break;
			}
		}
		Tile* siguiente = mapa -> getTilePorPixeles(xSiguiente,ySiguiente);
		printf("%d %d\n",xSiguiente,ySiguiente);
		if (siguiente != NULL){
			//retorno = siguiente;
		}
	}
	return retorno;;
}

void Entidad::setTileActual(Tile* tile) {
	this -> tileActual = tile;
}

void Entidad::setTileDestino(Tile* tile){
	this -> tileDestino = tile;
}