#include "Personaje.h"
#include "../net/PjeManager.h"
#include "corazon.h"
#include "botella.h"
#include "zapatos.h"
#include "flechas.h"
#include "bombas.h"
#include "granadas.h"
#include "varitas.h"
#include "escudo.h"
#include "../net/bitstream.h"
#include "../net/defines.h"
#include "../../Cliente/clientsocket.h"
extern PjeManager pjm;
extern ClientSocket sock;

/*
	Pre:- 

	Post: Se calcula el deltaUpdatePosicion adecuado.
*/
unsigned int calcularDeltaUptdatePosicionAdecuado(double velocidad){
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
	//seteo de velocidad
	this -> velocidad = VELOCIDAD_DEFAULT/((double)CLOCKS_PER_SEC);
	//deltaUpdatePosicion
	this -> deltaUpdatePosicion = calcularDeltaUptdatePosicionAdecuado(this -> velocidad);
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
	this->setRadio(125);
	this->terremoto=false;
	this->hielo=false;
	this->energia=this->ENERGIA_TOTAL;
	this->magia=this->MAGIA_TOTAL;
	this->flechas=0;
	this->bombas=0;
	this->granadas=0;
	this->varita=false;
	this->energiaEscudo=0;
	this -> cambioDireccionHabilitado = true;
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
	this->nickname = "untitled";
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
	init("WTF?", name , wTiles , hTiles , fps , delay , 
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
void Personaje::init(const std::string& nickname, const std::string& name, 
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
	this->nickname = nickname;
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

unsigned int Personaje::actualizarPosicion(Mapa* mapa) {
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
void Personaje::calcularPosicionTentativa(unsigned int direccion , 
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
Tile* Personaje::obtenerTileAncla(const int posX , const int posY , 
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
void Personaje::mover(Tile* tileDestino , Mapa* mapa) {
	if (tileDestino != NULL) {
		this -> tileDestino = tileDestino;
		this -> cambioDireccionHabilitado = true;
	}
}

void Personaje::actualizarImagen(const unsigned int direccion){
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	if (imagenPersonaje != NULL){
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
bool Personaje::isCaminable(){
	return true;
}

/*
	Pre: La instancia ha sido creada.
	Post: Se retorna verdadero si se puede ocupar el tile ocupado 
	por dicha instancia.
*/
bool Personaje::isCaminable(Tile* tile){
	return ( (tile != NULL) && (this -> tileAncla != NULL) && (tile == this -> tileAncla) );
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
	unsigned int retorno = Personaje::MOVER_EN_CURSO;
	if (this -> imagen != NULL) {		
		ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
		if (imagenPersonaje != NULL){
			if (imagenPersonaje -> isImagenFreezada()) {
				retorno = Personaje::FREEZAR_EN_CURSO;
			} else {
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
	} else {
		this -> tileDestino = NULL;
		unsigned int direccionAtaque = ImagenPersonaje::ATAQUE_DIRECCION_ACTUAL;
		ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
		if (imagenPersonaje != NULL){
			imagenPersonaje -> setAccion(direccionAtaque);
		}
	}
	return retorno;
}

/*
	Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
	dirección del ataque.

	Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

*/
unsigned int Personaje::defender(Tile* tileDestino , Mapa* mapa) {
	unsigned int retorno = Personaje::DEFENDER_COMPLETADO;
	// chequeo que el tileDestino y el mapa sean diferentes de null
	if ( (tileDestino != NULL) && (mapa != NULL)) {
		this -> tileDestino = NULL;
		unsigned int direccionAtaque = ImagenPersonaje::DEFENSA_DIRECCION_ACTUAL;
		int deltaX = tileDestino -> getX() - posX;
		int deltaY = tileDestino -> getY() - posY;
		//calculo de direccion
		if (deltaX > 0){
			if(deltaY < 0){
				direccionAtaque = ImagenPersonaje::DEF_NORESTE;
			} else{
				if(deltaY == 0){
					direccionAtaque = ImagenPersonaje::DEF_ESTE;
				} else {
					direccionAtaque = ImagenPersonaje::DEF_SURESTE;
				}
			}
			ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
			if (imagenPersonaje != NULL){
				imagenPersonaje -> setAccion(direccionAtaque);
			}
		}else {
			if (deltaX < 0) {
				if (deltaY < 0){
					direccionAtaque = ImagenPersonaje::DEF_NOROESTE;
				}else{
					if (deltaY == 0) {
						direccionAtaque = ImagenPersonaje::DEF_OESTE;
					}else {
						direccionAtaque = ImagenPersonaje::DEF_SUROESTE;
					}
				}
			}else{
				if (deltaY < 0){
					direccionAtaque = ImagenPersonaje::DEF_NORTE;
				} else{
					if(deltaY > 0){
						direccionAtaque = ImagenPersonaje::DEF_SUR;
					}
				}
			}
			ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
			if (imagenPersonaje != NULL){
				imagenPersonaje -> setAccion(direccionAtaque);
			}
		}
	} else {
		this -> tileDestino = NULL;
		unsigned int direccionDefensa = ImagenPersonaje::DEFENSA_DIRECCION_ACTUAL;
		ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
		if (imagenPersonaje != NULL){
			imagenPersonaje -> setAccion(direccionDefensa);
		}
	}
	return retorno;
}

/*
	Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
	dirección del ataque.

	Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

*/
void Personaje::freezar(){
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	if (imagenPersonaje != NULL){
		imagenPersonaje -> setAccion(ImagenPersonaje::FREEZAR_ACCION_ACTUAL);
		this -> entidadDesconectada = imagenPersonaje -> isImagenFreezada();
	}
}

void Personaje::freezar(bool valor){
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	if (imagenPersonaje != NULL){
		if(valor) {
			imagenPersonaje -> setAccion(ImagenPersonaje::FREEZAR_ACCION_ACTUAL);
		}else{
			imagenPersonaje -> setAccion(ImagenPersonaje::DESFREEZAR_ACCION_ACTUAL);
		}
		this -> entidadDesconectada = valor;
	}
}

/*
*/
void Personaje::muerte() {
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	this -> tileDestino = NULL;
	if (imagenPersonaje != NULL){
		imagenPersonaje -> setAccion(ImagenPersonaje::MUERTE);
	}
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

const std::string& Personaje::getNick() const {
	return this->nickname;
}

bool Personaje::verificarAncla(Tile* ancla) {
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

void Personaje::dañarPersonaje(char daño) {
	this->energia-=daño;
	if (this->energia<=0) {
		this->energia=0;
		//Muere
	}
}

void Personaje::dañar(char daño) {
	int dañoEscudo=daño/2;
	int dañoPersonaje=daño-dañoEscudo;
	//Tiene escudo para defenderse
	if (this->energiaEscudo>0) {
		//Veo si puede absorver todo lo que corresponde al escudo
		if (this->energiaEscudo>=dañoEscudo) {
			this->energiaEscudo-=dañoEscudo;
			this->dañarPersonaje(dañoPersonaje);
		} else {
			dañoPersonaje+=(dañoEscudo-this->energiaEscudo);
			this->energiaEscudo=0; 
			//se destruyo el escudo
			this->dañarPersonaje(dañoPersonaje);
		}
	} else {
		//No tiene escudo
		this->dañarPersonaje(daño);
	}
}
		

void Personaje::chocarConLampara() {
	this->aumentarRadio(0.25);
	//Falta notificar al servidor
}

//Mejorar: recorrer todos los tiles y colocarlos como visitados
void Personaje::chocarConMapa() {
	this->aumentarRadio(100.0);
	//Falta notificar al servidor
}

void Personaje::aumentarVelocidad(char porcentaje) {
	int aumento = porcentaje+100;
	this->velocidad*=(aumento/100);
}

void Personaje::chocarConZapatos(Zapatos* zapatos) {
	this->aumentarVelocidad(zapatos->getAumentoVelocidad());
	//Falta notificar al servidor
}

void Personaje::utilizarTerremoto(Mapa* mapa, PjeManager* pjm, ClientSocket* sock) {
	int radio=this->RADIO_HECHIZO;
	char dañoRealizado;
	Tile* tilePersonaje;
	int xPersonaje,yPersonaje;
	this->terremoto=true; //hardcodeo para no tener que agarrar item primero
	if ((terremoto) && (this->magia>=this->MAGIA_HECHIZO)) {
		std::cout << "Uso terremoto\n";
		BitStream bs;
		bs << PROTO::USE_ITEM << this->nickname << ITEM::TERREMOTO;
		sock->send(bs.str());
		this->magia-=this->MAGIA_HECHIZO;
		int xActual = this->getPosicion(mapa)->getU();
		int yActual = this->getPosicion(mapa)->getV();
		for (int i=xActual-radio;i<=xActual+radio;i++) {
			for (int j=yActual-radio;j<=yActual+radio;j++) {
				//Veo si algun personaje se encuentra en el radio
				for (auto it=pjm->getPjes().begin();it!=pjm->getPjes().end();it++) {
					tilePersonaje=it->second.getPosicion(mapa);
					xPersonaje=tilePersonaje->getU();
					yPersonaje=tilePersonaje->getV();
					if ((i==xPersonaje) && (j==yPersonaje)) {
						srand (time(NULL));
						dañoRealizado=rand()%(this->ENERGIA_TOTAL+1);
						std::cout << "se danio a " << it->first << " con el terremoto, total de " << (int)dañoRealizado << endl;
						it->second.dañar(dañoRealizado);
						bs.clear();
						bs << PROTO::DAMAGE << this->nickname << it->first << dañoRealizado;
						sock->send(bs.str());
					}
				}
			}
		}
		terremoto=false;
	}
}

void Personaje::utilizarHielo(Mapa* mapa, PjeManager* pjm) {
	int radio=this->RADIO_HECHIZO;
	Tile* tilePersonaje;
	int xPersonaje,yPersonaje;
	this->hielo=true; //hardcodeo para no tener que agarrar item primero
	if ((hielo) && (this->magia>=this->MAGIA_HECHIZO)) {
		std::cout << "Uso hechizo hielo\n";
		this->magia-=this->MAGIA_HECHIZO;
		int xActual = this->getPosicion(mapa)->getU();
		int yActual = this->getPosicion(mapa)->getV();
		for (int i=xActual-radio;i<=xActual+radio;i++) {
			for (int j=yActual-radio;j<=yActual+radio;j++) {
				//Veo si algun personaje se encuentra en el radio
				for (auto it=pjm->getPjes().begin();it!=pjm->getPjes().end();it++) {
					tilePersonaje=it->second.getPosicion(mapa);
					xPersonaje=tilePersonaje->getU();
					yPersonaje=tilePersonaje->getV();
					if ((i==xPersonaje) && (j==yPersonaje)) {
						it->second.freezar();
						std::cout << "Se congelo a " << it->first << " con el hechizo hielo" << endl;
						//Falta notificar al servidor
					}
				}
			}
		}
		terremoto=false;
	}
}

 void Personaje::chocarConCorazon(Corazon* corazon) {
		this->energia+=corazon->getEnergiaGanada();
		if (this->energia>this->ENERGIA_TOTAL)
			this->energia=this->ENERGIA_TOTAL;
}

 void Personaje::chocarConBotella(Botella* botella) {
	 this->magia+=botella->getMagiaGanada();
	 if (this->magia>this->MAGIA_TOTAL)
		 this->magia=this->MAGIA_TOTAL;
 }

 void Personaje::chocarConFlechas(Flechas* flechas) {
	 this->flechas+=flechas->getCantFlechas();
 }

 void Personaje::chocarConBombas(Bombas* bombas) {
	 this->bombas+=bombas->getCantBombas();
 }

 void Personaje::chocarConGranadas(Granadas* granadas) {
	 this->granadas+=granadas->getCantGranadas();
 }

 //Podria no recibir la varita, lo dejamos por si es necesario en un futuro
 void Personaje::chocarConVaritas(Varitas* varitas) {
	 this->varita=true;
 }

 void Personaje::chocarConEscudo(Escudo* escudo) {
	 this->energiaEscudo+=escudo->getEnergiaEscudo();
 }