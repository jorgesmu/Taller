#include "entidad.h"
#include "tile.h"
#include "mapa.h"


	/*
		Pre:-

		Post: Inicializa los atributos con los valores por defecto.
	*/
	void Entidad::inicializarAtributosEnValoresDefault() {
		//posicion
		this -> posX = 0;
		this -> posX = 0;
		//tile destino
		this -> tileDestino = NULL;
		//tile ancla
		this -> tileAncla = NULL;
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
		//seteo posicion
		this -> posX = 0;
		this -> posY = 0;
		//seteo como compartido
		this -> compartido = true;
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
		//agrego entidad al tile
		if (tile != NULL) {
			tile -> deleteEntidad(this);
			tile -> addEntidad(this);
			//seteo posicion
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
		}
		//tile destino
		this -> tileDestino = tile;
		//tile ancla
		this -> tileAncla = tile;
		//seteo de velocidad
		this -> velocidad = velocidad;
		//tiempo siguiente update
		this -> tiempoProximoUpdate = clock();
		//seteo como compartido
		this -> compartido = true;
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
		//agrego entidad al tile
		if (tile != NULL) {
			tile -> deleteEntidad(this);
			tile -> addEntidad(this);
			//seteo posicion
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
		}
		//tile destino
		this -> tileDestino = tile;
		//tile ancla
		this -> tileAncla = tile;
		//tiempo siguiente update
		this -> tiempoProximoUpdate = clock();
		//seteo como compartido
		this -> compartido = true;
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
		this -> tileDestino = NULL;
		this -> tileAncla = NULL;
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
				int posX;
				int posY;
				if (compartido){
					posX = tileX - (int)(camara -> getX()) - this -> pixel_ref_x;
					posY = tileY - (int)(camara -> getY()) - this -> pixel_ref_y;
				} else {
					posX = this -> posX - (int)(camara -> getX()) - this -> pixel_ref_x;
					posY = this -> posY - (int)(camara -> getY()) - this -> pixel_ref_y;
				}
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
		if (tileDestino != NULL) {
			int deltaX = this -> tileDestino -> getX() - posX;
			int deltaY = this -> tileDestino -> getY() - posY;
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
		Tile* tileActual = mapa -> getTilePorPixeles(this -> posX , this -> posY);
		// Si la direccion es centro se dirige al mismo
		if (tileActual == tileDestino) {
			this -> actualizarImagen(direccion);
		//Si es otro Tile se actualizacion posiciones hacia el mismo
		} else {
			// Calculo posicion siguiente (tomando en cuenta pixel de referencia y la posicion actual)
			int posPixelSiguienteX = 0;
			int posPixelSiguienteY = 0;
			this -> calcularPosicionTentativa(direccion , &posPixelSiguienteX , &posPixelSiguienteY);
			// Obtengo el tile siguiente
			Tile* tileSiguiente = mapa -> getTilePorPixeles(posPixelSiguienteX , posPixelSiguienteY);
			// Si el tileSiguiente es no nulo continua, sino no hace nada
			printf("Posicion Siguiente %d %d %u\n" , posPixelSiguienteX  , posPixelSiguienteY,direccion);
			if (tileSiguiente != NULL){
				// Obtengo el tile del ancla nueva
				Tile* tileAnclaSiguiente = this->obtenerTileAncla(
										posPixelSiguienteX , posPixelSiguienteY , 
										direccion , mapa);
				
				this -> posX = posPixelSiguienteX;
				this -> posY = posPixelSiguienteY; 
				
				// Ancla Siguiente distinta a ancla actual
				if (tileAnclaSiguiente != NULL){
					if(tileAnclaSiguiente != this -> tileAncla ) {
						if (tileAncla != NULL) {
							this -> tileAncla -> deleteEntidad(this);
						}
						this -> tileAncla = tileAnclaSiguiente;
						this -> tileAncla -> addEntidad(this);
					}
				} else{
					Tile* tileActual = mapa -> getTilePorPixeles(this -> posX , this ->posY );
					if (tileActual != NULL) {
						if (tileAncla != NULL) {
							this -> tileAncla -> deleteEntidad(this);
						}
						this -> tileAncla = tileActual;
						this -> tileAncla -> addEntidad(this);
					}
				}
			}
		}
	}
	
	/* 
		El offset X Y sin tener en cuenta el pixel de origen
	*/
	void Entidad::calcularPosicionTentativa(unsigned int direccion , 
									int* offsetTentativoX , int* offsetTentativoY){
		*offsetTentativoX = posX;
		*offsetTentativoY = posY;
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
	Tile* Entidad::obtenerTileAncla(const int posX , const int posY , 
													const unsigned int direccion , Mapa* mapa){
	Tile* retorno =NULL;
	int posImagenX = posX + this -> imagen -> getAncho()/2 - this -> pixel_ref_x;
	int posImagenY = posY + this -> imagen -> getAlto() - this -> pixel_ref_y;
	retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
	//printf("%d %d\n",posImagenX,posImagenY);
	return retorno;;
}

void Entidad::setTileActual(Tile* tile) {
	if (tile != NULL){
		this -> posX = tile -> getX();
		this -> posY = tile -> getY();
		this -> tileAncla = tile;
		this -> tileDestino = tile;
	}
}

void Entidad::setTileDestino(Tile* tile){
	this -> tileDestino = tile;
}