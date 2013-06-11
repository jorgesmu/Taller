#include "arma.h"
#include "../net/bitstream.h"
#include "../net/defines.h"
#include "../../Cliente/clientsocket.h"
#include "../net/PjeManager.h"
#include "../../source/utilities/soundman.h"
#include "aux_func.h"
extern ClientSocket sock;
extern SoundMan soundman;

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.
*/
Arma::Arma(const std::string& name,
		const unsigned int fps , const unsigned int delay ,
		const unsigned int velocidad ,
		const int pixel_ref_sprites_primario_x , const int pixel_ref_sprites_primario_y,
		Tile* tile, 
		ResMan& rm , const int colorKey , 
		char danio_maximo , Personaje* propietario){
	
	// seteo del puntero a imagen
	this -> imagen = NULL;
	//carga de imagen y configuración inicial.
	init(name , fps , delay , velocidad,
		pixel_ref_sprites_primario_x , pixel_ref_sprites_primario_y,
		tile , rm , colorKey , danio_maximo , propietario);	
}

//Toma el item o si es una bomba tendría que reventar sobre ese personaje si no es el propietario
void Arma::chocarCon(Personaje* personaje){
	// agregar arma al personaje si correspondiese
	this -> propietario = personaje;
}

unsigned int Arma::getDanioMaximo(){
	return this -> danio_maximo;
}

Personaje* Arma::getPropietario() {
	return this -> propietario;
}

/*
	Pre:- 

	Post: Se calcula el deltaUpdatePosicion adecuado.
*/
unsigned int Arma::calcularDeltaUptdatePosicionAdecuado(double velocidad){
	clock_t deltaUpdateCalculado = 0;
	double deltaPosicion = 0;
	while (deltaPosicion < 1){
		deltaPosicion = deltaUpdateCalculado * velocidad;
		deltaUpdateCalculado++;
	}
	return deltaUpdateCalculado;
}

/*
	Pre:-

	Post: Inicializa los atributos con los valores por defecto.
*/
void Arma::inicializarAtributosEnValoresDefault() {
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
	//seteo de velocidad
	this -> velocidad = VELOCIDAD_DEFAULT/((double)CLOCKS_PER_SEC);
	//deltaUpdatePosicion
	this -> deltaUpdatePosicion = this -> calcularDeltaUptdatePosicionAdecuado(this -> velocidad);
	//tiempo siguiente update
	this -> tiempoProximoUpdate = clock();
	//seteo posicion
	this -> posX = 0;
	this -> posY = 0;
	//seteo como compartido
	this -> compartido = true;
	//seteo flag de actualizando posicion
	this -> actualizandoPosicion = false;
	this -> ordenBliteo = Entidad::ORDEN_PERSONAJE;
	this -> cambioDireccionHabilitado = true;
	// propietario y daño
	this -> propietario = propietario;
	this -> danio_maximo = danio_maximo; // energia que quita el arma
	if (this -> propietario != NULL){
		ImagenArma* imagenArma = static_cast<ImagenArma*> (this -> imagen);
		this -> tileDestino = NULL;
		if (imagenArma != NULL){
			imagenArma -> setAccion(ImagenArma::INVISIBLE);
		}
	}
}

/*
	Pre: La instancia ha sido creada.
	 
	Post: Se ha destruido la instancia liberando los recursos asociados.
*/
Arma::~Arma() {
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
void Arma::init(const std::string& name,
				const unsigned int fps , const unsigned int delay ,
				const unsigned int velocidad ,
				const int pixel_ref_sprites_primario_x , const int pixel_ref_sprites_primario_y,
				Tile* tile, 
				ResMan& rm , const int colorKey , char danio_maximo , Personaje* propietario) {
	// Se destruyen imagenes previas
	if (this -> imagen != NULL) {
		delete(this -> imagen);
		this -> imagen = NULL;
	}
	// Se setean los atributos a sus valores por defecto.
	this -> inicializarAtributosEnValoresDefault();
	// Se carga la nueva imagen
	this -> imagen	= new ImagenArma(name.c_str() , 5 , delay , rm ,colorKey);
	this -> name = name;
	// Seteo del Surface
	this -> surf = this -> imagen -> getSurface();
	//Seteo de dimensiones
	this -> highInTiles = 1;
	this -> widthInTiles = 1;
	//pixel de referencia
	this -> pixel_ref_x = pixel_ref_sprites_primario_x;
	this -> pixel_ref_y = pixel_ref_sprites_primario_y;
	//tile destino
	this -> tileDestino = tile;
	//tile ancla
	this -> tileAncla = tile;
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
	this -> setNoDibujaFueraDelRadio();
	this -> ordenBliteo = Entidad::ORDEN_PERSONAJE;
	// seteo de danio
	this -> danio_maximo = danio_maximo;
	// seteo el propietario
	this -> propietario = propietario;
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
unsigned int Arma::calcularDireccion(Mapa* mapa){
	const int TOLERANCIA_SUPERIOR = 4;
	const int TOLERANCIA_INFERIOR = -3;
	unsigned int retorno;
	if (tileDestino != NULL) {
		int deltaX = this -> tileDestino -> getX() - posX;
		int deltaY = this -> tileDestino -> getY() - posY;
		//calculo de direccion
		if (deltaX > TOLERANCIA_SUPERIOR){
			if(deltaY < TOLERANCIA_INFERIOR){
				retorno = Personaje::NORESTE;
			} else{
				if((deltaY >= TOLERANCIA_INFERIOR) &&(deltaY <= TOLERANCIA_SUPERIOR)){
					retorno = Personaje::ESTE;
				} else {
					retorno = Personaje::SURESTE;
				}
			}
		}else {
			if (deltaX < TOLERANCIA_INFERIOR) {
				if (deltaY < TOLERANCIA_INFERIOR){
					retorno = Personaje::NOROESTE;
				}else{
					if ((deltaY >= TOLERANCIA_INFERIOR) &&(deltaY <= TOLERANCIA_SUPERIOR)) {
						retorno = Personaje::OESTE;
					}else {
						retorno = Personaje::SUROESTE;
					}
				}
			}else{
				if (deltaY < TOLERANCIA_INFERIOR){
					retorno = Personaje::NORTE;
				} else{
					if((deltaY >= TOLERANCIA_INFERIOR) && (deltaY <= TOLERANCIA_SUPERIOR)){
						retorno = Personaje::CENTRO;
					}else{
						retorno = Personaje::SUR;
					}
				}
			}
		}
	}
	return retorno;
}

unsigned int Arma::actualizarPosicion(Mapa* mapa) {
	unsigned int retorno = Personaje::MOVER_ERROR;
	//Pongo en verdadero el flag de actualizar posicion
	this -> actualizandoPosicion = true;		
	//Calculo de direccion
	unsigned int direccion = this -> calcularDireccion(mapa);
	// Si la direccion llego al tile
	if(this -> tileDestino != NULL) {
		// tileDestino distinto a la posicion exacta del tileActual
		if( (this -> posX != tileDestino -> getX()) || (this -> posY != tileDestino -> getY())){
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
		}
		if ((direccion == CENTRO) || 
			((this -> posX == tileDestino -> getX()) && 
			(this -> posY == tileDestino -> getY()))) {
			retorno = Personaje::MOVER_COMPLETADO;
			this -> tileDestino = NULL;
			// Se pasa centro para que se ponga estatico en la direccion actual
			this -> actualizarImagen(Personaje::CENTRO);
		} else {
			this -> actualizarImagen(direccion);
		}
	}
	this -> actualizandoPosicion = false;	
	return retorno;
}
	
/* 
	El offset X Y sin tener en cuenta el pixel de origen
*/
void Arma::calcularPosicionTentativa(unsigned int direccion , 
								int* offsetTentativoX , int* offsetTentativoY){
	*offsetTentativoX = posX;
	*offsetTentativoY = posY;
	unsigned int avance = ceil((this->velocidad)*(clock() - this->tiempoProximoUpdate + this ->deltaUpdatePosicion)
							*this->velocidad);
	switch (direccion){
			case NORTE :  {
				(*offsetTentativoY)-= avance;
				break;
			}
			case NORESTE : {
				(*offsetTentativoX)+= (2*avance);
				(*offsetTentativoY)-= avance;
				break;
			}
			case ESTE : {
				(*offsetTentativoX)+= (2*avance);
				break;
			}
			case SURESTE : {
				(*offsetTentativoX)+= (2*avance);
				(*offsetTentativoY)+= avance;
				break;
			}
			case SUR : {
				(*offsetTentativoY)+= avance;
				break;
			}
			case SUROESTE : {
				(*offsetTentativoX)-= (2*avance);
				(*offsetTentativoY)+= avance;
				break;
			}
			case OESTE : {
				(*offsetTentativoX)-= (2*avance);
				break;
			}
			case NOROESTE : {
				(*offsetTentativoX)-= (2*avance);
				(*offsetTentativoY)-= avance;
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
Tile* Arma::obtenerTileAncla(const int posX , const int posY , 
												const unsigned int direccion , Mapa* mapa){
	Tile* retorno = NULL;
	// Sur
	int posImagenX = posX;
	int posImagenY = posY + Tile::TILE_ALTO + Personaje::MARGEN_ANCLA_Y;
	retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
	if (retorno == NULL){
		// Sureste
		posImagenY = posY + Tile::TILE_ALTO;
		retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
		if (retorno == NULL) {
			// Este
			posImagenX = posX - Tile::TILE_ANCHO;
			posImagenY = posY + Tile::TILE_ALTO/2;
			if(retorno == NULL) {
				// tile Suroeste
				posImagenX = posX - Tile::TILE_ANCHO/2;
				posImagenY = posY + Tile::TILE_ALTO/2;
				retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
				if(retorno == NULL) {
					// tile Actual
					posImagenX = posX;
					posImagenY = posY;
					retorno = mapa -> getTilePorPixeles(posImagenX , posImagenY);
				}
			}

		}
	}
	return retorno;	
}

void Arma::setTileActual(Tile* tile) {
	if ( (tile != NULL) && (!this->actualizandoPosicion)){
		if(this -> tileAncla != tile) {
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
			this -> tileDestino = tile;
		}
		this -> tileAncla = tile;
	}
}

void Arma::setTileActual(Tile* tile , Mapa* mapa){
	this -> setTileActual(tile);
}

void Arma::setTileDestino(Tile* tile){
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
void Arma::mover(Tile* tileDestino , Mapa* mapa) {
	if (tileDestino != NULL) {
		this -> tileDestino = tileDestino;
		this -> cambioDireccionHabilitado = true;
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
void Arma::mover(Tile* tileDestino) {
	if (tileDestino != NULL) {
		this -> tileDestino = tileDestino;
		this -> cambioDireccionHabilitado = true;
	}
}

void Arma::actualizarImagen(const unsigned int direccion){
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	if ((imagenPersonaje != NULL) && (this -> propietario == NULL)){
		if (direccion == CENTRO) {
			imagenPersonaje -> setAccion(ImagenPersonaje::ESTATICO_DIRECCION_ACTUAL);
		} else {
			if (this -> cambioDireccionHabilitado) {
				imagenPersonaje -> setAccion(direccion);
				this -> cambioDireccionHabilitado = false;
			}
		}
	}
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna verdadero si se puede ocupar el tile ocupado 
	por dicha instancia.
*/
bool Arma::isCaminable(){
	return true;
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna verdadero si se puede ocupar el tile ocupado 
	por dicha instancia.
*/
bool Arma::isCaminable(Tile* tile){
	return ( (tile != NULL) && (this -> tileAncla != NULL) && (tile == this -> tileAncla) );
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna el tile donde se encuentra la instancia.
*/
Tile* Arma::getPosicion(Mapa* mapa){
	Tile* retorno=NULL;
	if (mapa != NULL) {
		retorno=mapa -> getTilePorPixeles(this -> posX , this -> posY);
	}
	return retorno;
}

// Actualiza las cosas internas, si hubiese
unsigned int Arma::update(Mapa* mapa) {
	unsigned int retorno = Arma::MOVER_EN_CURSO;
	if (this -> propietario != NULL) {
		this -> posX = this -> propietario -> getX();
		this -> posY = this -> propietario -> getY();
	}
	if (this -> imagen != NULL) {		
		ImagenArma* imagenArma = static_cast<ImagenArma*> (this -> imagen);
		if (imagenArma != NULL){
			if (this -> tiempoProximoUpdate <= clock()){	
				if (this -> tileDestino != NULL) {
					//actualizacion de posicion
					retorno = this -> actualizarPosicion(mapa);
				} else {
					retorno = Personaje::MOVER_COMPLETADO; // ESPERANDO_ACCION
				}
				this -> tiempoProximoUpdate = clock() + this -> deltaUpdatePosicion;
			}			
		}
	}
	if(this -> imagen != NULL) {
		this -> surf = this -> imagen -> getSurface();
	}
	return retorno;
}

/*
	Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
	dirección del ataque.

	Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

*/

unsigned int Arma::atacar(Mapa* mapa, Tile* tileDestino , Personaje* personaje) {
	unsigned int retorno = Personaje::ATACAR_COMPLETADO;
	// chequeo que el tileDestino y el mapa sean diferentes de null
	if ( (tileDestino != NULL) && (mapa != NULL) && (personaje != NULL)) {
		Tile* tileActualizado = mapa->getTilePorPixeles(personaje -> getX() , personaje -> getY());	
		if ( tileActualizado == tileDestino){ 
			this -> tileDestino = NULL;
			unsigned int direccionAtaque = ImagenArma::ATAQUE_DIRECCION_ACTUAL;
			int deltaX = tileDestino -> getX() - posX;
			int deltaY = tileDestino -> getY() - posY;
			//calculo de direccion
			if (deltaX > 0){
				if(deltaY < 0){
					direccionAtaque = ImagenArma::AT_NORESTE;
				} else{
					if(deltaY == 0){
						direccionAtaque = ImagenArma::AT_ESTE;
					} else {
						direccionAtaque = ImagenArma::AT_SURESTE;
					}
				}
					ImagenArma* imagenArma = static_cast< ImagenArma*> (this -> imagen);
				if (imagenArma != NULL){
					imagenArma -> setAccion(direccionAtaque);
					//ataque a personaje
					dañarPersonaje(personaje);
				}
			}else {
				if (deltaX < 0) {
					if (deltaY < 0){
						direccionAtaque = ImagenArma::AT_NOROESTE;
					}else{
						if (deltaY == 0) {
							direccionAtaque = ImagenArma::AT_OESTE;
						}else {
							direccionAtaque = ImagenArma::AT_SUROESTE;
						}
					}
				}else{
					if (deltaY < 0){
						direccionAtaque = ImagenArma::AT_NORTE;
					} else{
						if(deltaY > 0){
							direccionAtaque = ImagenArma::AT_SUR;
						}
					}
				}
				ImagenArma* imagenArma = static_cast<ImagenArma*> (this -> imagen);
				if (imagenArma != NULL){
					imagenArma -> setAccion(direccionAtaque);
					//ataque a personaje
					dañarPersonaje(personaje);
				}
			
			}
		}
		soundman.playSound("sword", 0, 0);
	} /*else {
		this -> tileDestino = NULL;
		unsigned int direccionAtaque = ImagenArma::ATAQUE_DIRECCION_ACTUAL;
		ImagenArma* imagenArma = static_cast<ImagenArma*> (this -> imagen);
		if (imagenArma != NULL){
			imagenArma -> setAccion(direccionAtaque);
			//ataque a personaje
			dañarPersonaje(personaje);
		}
	}*/
	return retorno;
}

/*
*/
void Arma::accionEspecial() {
	ImagenArma* imagenArma = static_cast<ImagenArma*> (this -> imagen);
	this -> tileDestino = NULL;
	if (imagenArma != NULL){
		imagenArma -> setAccion(ImagenArma::ACCION_ESPECIAL);
	}
}

// Por ahora retorna trivialmente la posicion en X
int Arma::getXAnclajeNiebla(){
	return this -> getX();
}

// Por ahora retorna trivialmente la posicion en Y
int Arma::getYAnclajeNiebla(){
	return this -> getY();
}

/*
	Pre: La instancia ha sido creada.

	Post: Se retorna verdadero si se puede ocupar el tile destino de la instancia, caso contrario
	se retorna falso.
*/
bool Arma::verificarDestinoCaminable(Mapa* mapa) {
	bool retorno = false;
	if ( (this -> tileDestino != NULL) && (mapa != NULL) ){
		// retorno = tileDestino -> isCaminable();
	}
	retorno = true; // TODO: Quitar luego cuando efectivamente verifique
	return retorno;
}

bool Arma::verificarAncla(Tile* ancla) {
	bool retorno = true;
	if (ancla != NULL) {
		std::vector<Entidad*> entidades = ancla->getEntidades();
		for(auto it = entidades.begin(); it != entidades.end(); ++it) {
			if (((*it)->getWidthInTiles() > 1) || ((*it)->getHighInTiles() > 1)){
				return false;
			}
		}
	}
	return retorno;
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
void Arma::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY){	
	if ( (this -> imagen != NULL) && (this -> surf != NULL) &&
		(camara != NULL)) {
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
			if (!this->color) {
				this -> surf -> blitGris(dest , posX ,posY);	
			} else {
				this -> surf -> blit(dest , posX ,posY);		
			}
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
void Arma::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY , 
					bool color){	
	if ( (this -> imagen != NULL) && (this -> surf != NULL) &&
		(camara != NULL)) {
		bool colorAux = this -> color;
		if ( (this -> highInTiles == 1) && (this -> widthInTiles == 1) ){
			colorAux = color;
		}
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
			if (!colorAux) {
				this -> surf -> blitGris(dest , posX ,posY);	
			} else {
				this -> surf -> blit(dest , posX ,posY);		
			}
		}
	}
}

void Arma::dañarPersonaje(Personaje* personajeObjetivo){
	int precision = intRand(0,100);
	if ( (personajeObjetivo != NULL) && (this -> propietario != NULL)){
		if (precision >= this ->propietario -> getPrecision()){
			srand (time(NULL));
			char danio=rand()%(this->danio_maximo+1);
			//int aux=danio;
			printf("\n%i\n",danio);
			personajeObjetivo -> dañar(danio);
			if (danio > 0) {
				// mensaje al servidor
				std::cout << "Ataque\n";
				BitStream bs;		
				bs << PROTO::DAMAGE << this-> propietario -> getNick() << personajeObjetivo->getNick() << danio;
				sock.send(bs.str());
				std::cout << "Ataque " << this-> propietario -> getNick() << "->" << personajeObjetivo->getNick() << " de " << (int)danio << endl;
			}
		}
	}
}