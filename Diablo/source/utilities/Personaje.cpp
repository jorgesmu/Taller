#include "Personaje.h"

/*
	Pre:-

	Post: Inicializa los atributos con los valores por defecto.
*/
void Personaje::inicializarAtributosEnValoresDefault() {
	//posicion
	this -> posX = 0;
	this -> posY = 0;
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
	//deltaUpdatePosicion
	this -> deltaUpdatePosicion = Personaje::BASE_DE_TIEMPO/Personaje::VELOCIDAD_DEFAULT;
	this -> velocidad = VELOCIDAD_DEFAULT;
	//tiempo siguiente update
	this -> tiempoProximoUpdate = clock();
	//seteo posicion
	this -> posX = 0;
	this -> posY = 0;
	//seteo como compartido
	this -> compartido = true;
	//seteo flag de actualizando posicion
	this -> actualizandoPosicion = false;
}

/*
	Pre:-
		 
	Post: Se ha creado la instancia con los valores por defecto.
*/
Personaje::Personaje(){
	// seteo del puntero a imagen
	this -> imagen = NULL;
	// Se setean los atributos a sus valores por defecto.
	this -> inicializarAtributosEnValoresDefault();
}

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.

*/
Personaje::Personaje(const std::string& name, 
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
	Pre: La instancia ha sido creada.
	 
	Post: Se ha destruido la instancia liberando los recursos asociados.
*/
Personaje::~Personaje() {
	if (this -> imagen != NULL) {
		delete(this -> imagen);
		this -> imagen = NULL;
	}
	this -> surf = NULL;
	this -> tileAncla = NULL;
}

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.

*/
void Personaje::init(const std::string& name, 
		const unsigned int wTiles , const unsigned int hTiles , 
		const unsigned int fps , const unsigned int delay , 
		const unsigned int  altoSprite , const unsigned int anchoSprite ,
		const unsigned int velocidad ,
		int pixel_ref_x ,int pixel_ref_y,
		Tile* tile, 
		ResMan& rm , const int colorKey) {
	// Se destruyen imagenes previas
	if (this -> imagen != NULL) {
		delete(this -> imagen);
		this -> imagen = NULL;
	}
	// Se setean los atributos a sus valores por defecto.
	this -> inicializarAtributosEnValoresDefault();
	// Se carga la nueva imagen
	this -> imagen	= new ImagenPersonaje(name.c_str() , altoSprite , 
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
	//tile destino
	this -> tileDestino = tile;
	//tile ancla
	this -> tileAncla = tile;
	//deltaUpdatePosicion
	this -> deltaUpdatePosicion = Personaje::BASE_DE_TIEMPO/velocidad;
	this -> velocidad = velocidad;
	//tiempo siguiente update
	this -> tiempoProximoUpdate = clock();
	//agrego entidad al tile
	if (tile != NULL) {
		if(this -> tileAncla != NULL){
			this -> tileAncla -> deleteEntidad(this);
		}
		this -> tileAncla = tile;
		this -> tileAncla -> addEntidad(this);
		//seteo posicion
		this -> posX = tile -> getX();
		this -> posY = tile -> getY();
	}
	//seteo como compartido
	this -> compartido = false;
	//seteo flag de actualizando posicion
	this -> actualizandoPosicion = false;
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
unsigned int Personaje::calcularDireccion(Mapa* mapa){
	const int TOLERANCIA_SUPERIOR = 3;
	const int TOLERANCIA_INFERIOR = -3;
	if (tileDestino != NULL) {
		int deltaX = this -> tileDestino -> getX() - posX;
		int deltaY = this -> tileDestino -> getY() - posY;
		//calculo de direccion
		if (deltaX > TOLERANCIA_SUPERIOR){
			if(deltaY < TOLERANCIA_INFERIOR){
				return Personaje::NORESTE;
			} else{
				if((deltaY >= TOLERANCIA_INFERIOR) &&(deltaY <= TOLERANCIA_SUPERIOR)){
					return Personaje::ESTE;
				} else {
					return Personaje::SURESTE;
				}
			}
		}else {
			if (deltaX < TOLERANCIA_INFERIOR) {
				if (deltaY < TOLERANCIA_INFERIOR){
					return Personaje::NOROESTE;
				}else{
					if ((deltaY >= TOLERANCIA_INFERIOR) &&(deltaY <= TOLERANCIA_SUPERIOR)) {
						return Personaje::OESTE;
					}else {
						return Personaje::SUROESTE;
					}
				}
			}else{
				if (deltaY < TOLERANCIA_INFERIOR){
					return Personaje::NORTE;
				} else{
					if((deltaY >= TOLERANCIA_INFERIOR) && (deltaY <= TOLERANCIA_SUPERIOR)){
						return Personaje::CENTRO;
					}else{
						return Personaje::SUR;
					}
				}
			}
		}
	}
	return CENTRO;
}

unsigned int Personaje::actualizarPosicion(Mapa* mapa) {
	unsigned int retorno = Personaje::MOVER_ERROR;
	//Pongo en verdadero el flag de actualizar posicion
	this -> actualizandoPosicion = true;		
	//Calculo de direccion
	unsigned int direccion = this -> calcularDireccion(mapa);
	// Si la direccion llego al tile
	if(this -> tileDestino != NULL) {
		if( (this -> posX != tileDestino -> getX()) || (this -> posY != tileDestino -> getY())){
			if ( verificarDestinoCaminable(mapa) ){

				retorno = Personaje::MOVER_EN_CURSO;
				// Calculo posicion siguiente (tomando en cuenta pixel de referencia y la posicion actual)
				int posPixelSiguienteX = 0;
				int posPixelSiguienteY = 0;
				this -> calcularPosicionTentativa(direccion , &posPixelSiguienteX , &posPixelSiguienteY);
				// Obtengo el tile siguiente
				Tile* tileSiguiente = mapa -> getTilePorPixeles(posPixelSiguienteX , posPixelSiguienteY);
				// Si el tileSiguiente es no nulo continua, sino no hace nada
				if (tileSiguiente != NULL){
					// Obtengo el tile del ancla nueva
					Tile* tileAnclaSiguiente = this -> obtenerTileAncla(
									posPixelSiguienteX , posPixelSiguienteY , 
									direccion , mapa);
					if (tileAnclaSiguiente != NULL) {
						Tile* tileActual = mapa -> getTilePorPixeles(this -> posX , this -> posY);
						// seteo de la posicion siguiente
						this -> posX = posPixelSiguienteX;
						this -> posY = posPixelSiguienteY;
						// agrego la entidad al ancla siguiente
						if (tileAnclaSiguiente != this -> tileAncla ) {
							if (tileAncla != NULL) {
								this -> tileAncla -> deleteEntidad(this);
							}
							this -> tileAncla = tileAnclaSiguiente;
							this -> tileAncla -> addEntidad(this);	
						} 
						// agregado de entidad al tile nuevo
						if (tileSiguiente != tileActual) {	
							if (tileActual != NULL) {
								if (this -> tileAncla != tileActual){
									tileActual -> deleteEntidad(this);
								} 
								if (this -> tileAncla != tileSiguiente){
									tileSiguiente -> addEntidad(this);
								}
							} else {
								if (this -> tileAncla != tileSiguiente){
									tileSiguiente -> addEntidad(this);
								}
							}	
						}
					} else{
						Tile* tileActual = mapa -> getTilePorPixeles(this -> posX , this -> posY);
						// seteo de la posicion siguiente
						this -> posX = posPixelSiguienteX;
						this -> posY = posPixelSiguienteY;
						// agrego la entidad al ancla siguiente
						if (tileAncla != NULL) {
							this -> tileAncla -> deleteEntidad(this);
						}
						// seteo como tileAncla el tileSiguiente
						this -> tileAncla = tileSiguiente;
						this -> tileAncla -> addEntidad(this);
						// agregado de entidad al tile nuevo
						if (tileSiguiente != tileActual) {	
							if (tileActual != NULL) {
								tileActual -> deleteEntidad(this);
							} 		
						}
					}
				}
			} else {
				retorno = Personaje::MOVER_ERROR;
			}
		}
		if ((this -> posX == tileDestino -> getX()) && (this -> posY == tileDestino -> getY()) && (direccion == CENTRO)) {
			retorno = Personaje::MOVER_COMPLETADO;
			this -> tileDestino = NULL;
		}
	}
	this -> actualizandoPosicion = false;
	this -> actualizarImagen(direccion);
	return retorno;
}
	
/* 
	El offset X Y sin tener en cuenta el pixel de origen
*/
void Personaje::calcularPosicionTentativa(unsigned int direccion , 
								int* offsetTentativoX , int* offsetTentativoY){
	*offsetTentativoX = posX;
	*offsetTentativoY = posY;
	switch (direccion){
			case NORTE :  {
				(*offsetTentativoY)-=Personaje::DELTA_AVANCE;
				break;
			}
			case NORESTE : {
				(*offsetTentativoX)+=(2*Personaje::DELTA_AVANCE);
				(*offsetTentativoY)-=Personaje::DELTA_AVANCE;
				break;
			}
			case ESTE : {
				(*offsetTentativoX)+=(2*Personaje::DELTA_AVANCE);
				break;
			}
			case SURESTE : {
				(*offsetTentativoX)+=(2*Personaje::DELTA_AVANCE);
				(*offsetTentativoY)+=Personaje::DELTA_AVANCE;
				break;
			}
			case SUR : {
				(*offsetTentativoY)+=Personaje::DELTA_AVANCE;
				break;
			}
			case SUROESTE : {
				(*offsetTentativoX)-=(2*Personaje::DELTA_AVANCE);
				(*offsetTentativoY)+=Personaje::DELTA_AVANCE;
				break;
			}
			case OESTE : {
				(*offsetTentativoX)-=(2*Personaje::DELTA_AVANCE);
				break;
			}
			case NOROESTE : {
				(*offsetTentativoX)-=(2*Personaje::DELTA_AVANCE);
				(*offsetTentativoY)-=Personaje::DELTA_AVANCE;
				break;
			}
			case CENTRO : {
				(*offsetTentativoX)+= this -> tileDestino -> getX() - this -> posX;
				(*offsetTentativoY)+= this -> tileDestino -> getY() - this -> posY;
				break;
			}
	}
}
	
/*
	posX y posY en pixeles, es una posicion cualquiera en el mapa
*/
Tile* Personaje::obtenerTileAncla(const int posX , const int posY , 
												const unsigned int direccion , Mapa* mapa){
	Tile* retorno = NULL;
	int posImagenX = posX;
	int posImagenY = posY + Tile::TILE_ALTO + Personaje::MARGEN_ANCLA_Y;
	retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
	if (retorno == NULL){
		posImagenY = posY + Tile::TILE_ALTO;
		retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
		if (retorno == NULL) {
			posImagenX = posX - Tile::TILE_ANCHO;
			posImagenY = posY + Tile::TILE_ALTO/2;
			if(retorno == NULL) {
				posImagenX = posX - this -> pixel_ref_x;
				retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
			}
		}
	}
	return retorno;
}

void Personaje::setTileActual(Tile* tile) {
	if ( (tile != NULL) && (!this->actualizandoPosicion)){
		if(this -> tileAncla != tile) {
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
			this -> tileDestino = tile;
		}
		this -> tileAncla = tile;
	}
}

void Personaje::setTileActual(Tile* tile , Mapa* mapa){
	setTileActual(tile);
}

void Personaje::setTileDestino(Tile* tile){
	this -> tileDestino = tile;
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
void Personaje::mover(Tile* tileDestino) {
	if (tileDestino != NULL) {
		this -> tileDestino = tileDestino;
	}
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
void Personaje::mover(Tile* tileDestino , Mapa* mapa) {
	if (tileDestino != NULL) {
		this -> tileDestino = tileDestino;
	}
}

void Personaje::actualizarImagen(const unsigned int direccion){
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	if (imagenPersonaje != NULL){
		if (direccion == CENTRO) {
			imagenPersonaje -> setAccion(ImagenPersonaje::ESTATICO_DIRECCION_ACTUAL);
		} else {
			imagenPersonaje -> setAccion(direccion);
		}
		// Actualizacion del surface
		this -> surf = this -> imagen -> getSurface();
	}
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna verdadero si se puede ocupar el tile ocupado 
	por dicha instancia.
*/
bool Personaje::isCaminable(){
	return false;
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna verdadero si se puede ocupar el tile ocupado 
	por dicha instancia.
*/
bool Personaje::isCaminable(Tile* tile , Mapa* mapa){
	return ( (tile != NULL) && (this -> tileAncla != NULL) && (tile != this -> tileAncla) );
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna el tile donde se encuentra la instancia.
*/
Tile* Personaje::getPosicion(Mapa* mapa){
	Tile* retorno=NULL;
	if (mapa != NULL) {
		retorno=mapa -> getTilePorPixeles(this -> posX , this -> posY);
	}
	return retorno;
}

// Actualiza las cosas internas, si hubiese
unsigned int Personaje::update(Mapa* mapa) {
	unsigned int retorno = Personaje::ESPERANDO_ACCION;
	if (this -> tiempoProximoUpdate <= clock()){	
		if (this -> tileDestino != NULL) {
			//actualizacion de posicion
			retorno = this -> actualizarPosicion(mapa);
		} else {
			if(this -> imagen != NULL) {
				this -> surf = this -> imagen -> getSurface();
			}
		}
		this -> tiempoProximoUpdate = clock() + this -> deltaUpdatePosicion;
	}
	return retorno;
}

/*
	Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
	dirección del ataque.

	Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

*/
unsigned int Personaje::ataque(Tile* tileDestino , Mapa* mapa) {
	unsigned int retorno = Personaje::ATACAR_COMPLETADO;
	// chequeo que el tileDestino y el mapa sean diferentes de null
	if ( (tileDestino != NULL) && (mapa != NULL)) {
		this -> tileDestino = NULL;
		unsigned int direccionAtaque = ImagenPersonaje::ATAQUE_DIRECCION_ACTUAL;
		int deltaX = tileDestino -> getX() - posX;
		int deltaY = tileDestino -> getY() - posY;
		//calculo de direccion
		if (deltaX > 0){
			if(deltaY < 0){
				direccionAtaque = ImagenPersonaje::AT_NORESTE;
			} else{
				if(deltaY == 0){
					direccionAtaque = ImagenPersonaje::AT_ESTE;
				} else {
					direccionAtaque = ImagenPersonaje::AT_SURESTE;
				}
			}
			ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
			if (imagenPersonaje != NULL){
				imagenPersonaje -> setAccion(direccionAtaque);
			}
		}else {
			if (deltaX < 0) {
				if (deltaY < 0){
					direccionAtaque = ImagenPersonaje::AT_NOROESTE;
				}else{
					if (deltaY == 0) {
						direccionAtaque = ImagenPersonaje::AT_OESTE;
					}else {
						direccionAtaque = ImagenPersonaje::AT_SUROESTE;
					}
				}
			}else{
				if (deltaY < 0){
					direccionAtaque = ImagenPersonaje::AT_NORTE;
				} else{
					if(deltaY > 0){
						direccionAtaque = ImagenPersonaje::AT_SUR;
					}
				}
			}
			ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
			if (imagenPersonaje != NULL){
				imagenPersonaje -> setAccion(direccionAtaque);
			}
		}
		printf("Direccion Ataque %d \n", direccionAtaque);
	}
	return retorno;
}

// Por ahora retorna trivialmente la posicion en X
int Personaje::getXAnclajeNiebla(){
	return this -> getX();
}

// Por ahora retorna trivialmente la posicion en Y
int Personaje::getYAnclajeNiebla(){
	return this -> getY();
}

/*
	Pre: La instancia ha sido creada.

	Post: Se retorna verdadero si se puede ocupar el tile destino de la instancia, caso contrario
	se retorna falso.
*/
bool Personaje::verificarDestinoCaminable(Mapa* mapa) {
	bool retorno = false;
	if ( (this -> tileDestino != NULL) && (mapa != NULL) ){
		// retorno = tileDestino -> isCaminable();
	}
	retorno = true; // TODO: Quitar luego cuando efectivamente verifique
	return retorno;
}