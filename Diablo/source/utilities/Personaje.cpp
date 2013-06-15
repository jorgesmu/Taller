#include "../display/bar.h"
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
#include "lampara.h"
#include "mapaItem.h"
#include "terremoto.h"
#include "bolaDeCristal.h"
#include "hielo.h"
#include "golemItem.h"
#include "armaBomba.h"
#include "bandera.h"
#include "aux_func.h"
#include "../net/bitstream.h"
#include "../net/defines.h"
#include "../../Cliente/clientsocket.h"
#include "../utilities/Arma.h"
#include "../utilities/console.h"
#include "soundman.h"
#include "armaBomba.h"
#include "aux_func.h"
#include <sstream>
extern PjeManager pjm;
extern ClientSocket sock;
extern int start_pos_x,start_pos_y;
extern int estadoMovimiento;
extern Mapa mapa;
extern std::vector<EntidadFija*> entidades_cargadas;
extern ResMan resman;
extern Console consola;
extern SoundMan soundman;

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
	this->terremoto=10; //DESCOMENTAR
	this->hielo=0; //DESCOMENTAR
	this->energia=this->ENERGIA_TOTAL;
	this->magia=this->MAGIA_TOTAL;
	this->flechas=0;
	this->bombas=0; //DESCOMENTAR
	this->granadas=0;
	this->varita=false;
	this->energiaEscudo=0;
	this->vivo=true;
	this->precision=this->PRECISION_PERSONAJE;
	this -> cambioDireccionHabilitado = true;
	this -> bolaDeCristal = false;
	this -> espada = NULL;
	this->transmutacion = true; //DESCOMENTAR
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
	this->PosicionActualizada = false;
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
	if (this -> tileAncla != NULL){
		this -> tileAncla ->deleteEntidad(this);
		this -> tileAncla = NULL; 
	}
	
	// Se elimina la espada
	if (this -> espada != NULL) {
		delete(this -> espada);
		this -> espada = NULL;
	}
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
	this -> bolaDeCristal = false;
	// Se inicializa el arma default que es la espada
	this -> espada = new Arma("espada" , 2 , 500 , 11,
		Arma::PIXEL_REF_SPRITES_PRIMARIO_X , Arma::PIXEL_REF_SPRITES_PRIMARIO_Y,
		tile , rm , Imagen::COLOR_KEY , Arma::DANIO_MAXIMO_DEFAULT , this);
	// Inicializo la barra de energia
	bar.setColor(COLOR::YELLOW);
	bar.setSize(50, 6);
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
			float posPixelSiguienteX = 0;
			float posPixelSiguienteY = 0;
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
		// si se completo el movimiento
		if ((direccion == CENTRO) || 
			((this -> posX == tileDestino -> getX()) && 
			(this -> posY == tileDestino -> getY()))) {
			retorno = Personaje::MOVER_COMPLETADO;
			//seteo tile destino en nulo
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
	float* offsetTentativoX , float* offsetTentativoY){
		if(this->velocidad == 0){
			this ->velocidad = 105/1000;
		}
	int vel = 3;
	*offsetTentativoX = posX;
	*offsetTentativoY = posY;
	float avance;
	if(this ->velocidad != 0){
	avance= ((this->velocidad)*(clock() - this->tiempoProximoUpdate + this ->deltaUpdatePosicion)
							*this->velocidad);
	}else{
	avance= ((vel)*(clock() - this->tiempoProximoUpdate + this ->deltaUpdatePosicion)
							*vel);
	
	}
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
		if (this -> getArmaActiva() != NULL){
			this -> getArmaActiva() -> mover(tileDestino);
		}
	}
}

void Personaje::dejarItem(int x, int y, ResMan* resman){
		//meto el item que deja cuando muere
		int cant_items = 9;//cantidad de items que implementamos
		int rand;
		rand = intRand(0, cant_items);
				
		std::cout << "va a dejar el item del case " << rand << "\n";
		switch(rand){
			case 0:{
				Lampara lampara("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&lampara,&mapa);
				entidades_cargadas.push_back(&lampara);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::LAMPARA << x << y;
				sock.send(bs.str());
				break;
			}
			case 1:{
				MapaItem mapaItem("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&mapaItem,&mapa);
				entidades_cargadas.push_back(&mapaItem);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::MAPAITEM << x << y;
				sock.send(bs.str());
				break;
			}
			case 2:{
				Zapatos zapato("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&zapato,&mapa);
				entidades_cargadas.push_back(&zapato);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::ZAPATOS << x << y;
				sock.send(bs.str());
				break;
			}
   			case 3:{
				Terremoto terremoto("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&terremoto,&mapa);
				entidades_cargadas.push_back(&terremoto);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::TERREMOTO << x << y;
				sock.send(bs.str());
				break;
			}
   			case 4:{
				Escudo escudo("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&escudo,&mapa);
				entidades_cargadas.push_back(&escudo);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::ESCUDO << x << y;
				sock.send(bs.str());
				break;
			}
   			case 5:{
				BolaDeCristal bolaDeCristal("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&bolaDeCristal,&mapa);
				entidades_cargadas.push_back(&bolaDeCristal);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::BOLA_DE_CRISTAL << x << y;
				sock.send(bs.str());
				break;
			}
   			case 6:{
				GolemItem golem("cofre",1,1,true, 6 ,13,NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&golem,&mapa);
				entidades_cargadas.push_back(&golem);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::GOLEM << x << y;
				sock.send(bs.str());
				break;
			}
   			case 7:{
				Botella botella("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&botella,&mapa);
				entidades_cargadas.push_back(&botella);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::BOTELLA << x << y;
				sock.send(bs.str());
				break;
			}
   			case 8:{
				Corazon corazon("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&corazon,&mapa);
				entidades_cargadas.push_back(&corazon);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::CORAZON << x << y;
				sock.send(bs.str());
				break;
			}
   			case 9:{
				Hielo hielo("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
				mapa.getTile(x, y)->addEntidad(&hielo,&mapa);
				entidades_cargadas.push_back(&hielo);
				BitStream bs;
				bs << PROTO::LEAVE_ITEM << ITEM::HIELO << x << y;
				sock.send(bs.str());
				break;
			}
   			/*case 10:{
				ArmaBomba cofre("cofre",1,1,true, x , y, NULL,&mapa,*resman,Imagen::COLOR_KEY );
			}*/
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
		if (this -> getArmaActiva() != NULL){
			this -> getArmaActiva() -> mover(tileDestino);
		}
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
	return true;
	return ( (tile != NULL) && (this -> tileAncla != NULL) && (tile == this -> tileAncla) );
}

bool Personaje::getBolaDeCristal(){
	return this->bolaDeCristal;
}

void Personaje::setBolaDeCristal(bool bolaDeCristal){
	this->bolaDeCristal = bolaDeCristal;
}

bool Personaje::tieneGolem(){
	return this->golem;
}

void Personaje::setGolem(bool agarroGolem){
	this->golem = agarroGolem;
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
	unsigned int retorno = Personaje::MOVER_COMPLETADO;
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
				} else {
					if (this -> tileDestino != NULL) {
						retorno = Personaje::MOVER_EN_CURSO;
					}
				}
			}
		}
	}
	if(this -> imagen != NULL) {
		this -> surf = this -> imagen -> getSurface();
	}
	if(this -> espada != NULL){
		this -> espada -> update(mapa);
	}
	return retorno;
}

void Personaje::updateRevivir() {
	//Logica de revivir
	if ((this->timerRevivir.isStarted()) && (this->timerRevivir.getTicks()>this->TIEMPO_REVIVIR)) {
		std::cout << "pidiendo al server q me reviva \n";
		BitStream bs;
		bs << PROTO::REQUEST_REV_POS << start_pos_x << start_pos_y;
		sock.send(bs.str());
		estadoMovimiento=MOV::ESPERANDO_OK;
		this->timerRevivir.stop();
	}
}

/*
	Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
	dirección del ataque.

	Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

*/
unsigned int Personaje::ataque(Tile* tileDestino , Mapa* mapa) {
	soundman.playSound("sword", this->getX(), this->getY());
	return this -> ataque(tileDestino , mapa, NULL);
}


/*
	Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
	dirección del ataque.

	Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

*/
unsigned int Personaje::ataque(Tile* tileDestino , Mapa* mapa , Personaje* personajeObjetivo) {
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
			// seteo accion imagen personaje
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
	// Ataque al personaje
	if (this -> getArmaActiva() != NULL){
		// pone en ejecucion la animacion y quita energia al personaje de acuerdo al arma
		this -> getArmaActiva() -> atacar(mapa,tileDestino,personajeObjetivo);
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
void Personaje::animacionMuerte() {
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	this -> tileDestino = NULL;
	if (imagenPersonaje != NULL){
		imagenPersonaje -> setAccion(ImagenPersonaje::MUERTE);
	}
}


/*
*/
void Personaje::animacionPiedra() {
	printf("\n---------------------------------Piedra---------------------------------------\n");
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	this -> tileDestino = NULL;
	if (imagenPersonaje != NULL){
		printf("\n---------------------------------Piedra---------------------------------------\n");
		imagenPersonaje -> setAccion(ImagenPersonaje::PIEDRA);
	}
}

void Personaje::animacionRevivir() {
	ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*> (this -> imagen);
	this -> tileDestino = NULL;
	if (imagenPersonaje != NULL){
		imagenPersonaje -> setAccion(ImagenPersonaje::EST_SUR);
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
	}
}

void Personaje::dañar(char daño) {
	char dañoEscudo=daño/2;
	char dañoPersonaje=daño-dañoEscudo;
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
	if (this->energia==0) {
		this->muere();
	}
}
		

void Personaje::chocarConLampara(Lampara* lampara) {
	this->getPosicion(&mapa)->deleteEntidad(lampara);
	this->aumentarRadio(lampara->getProporcionAumento());
	std::stringstream msj;
	msj << "Agarro Lampara";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	BitStream bs;
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
}

//Mejorar: recorrer todos los tiles y colocarlos como visitados
void Personaje::chocarConMapa(MapaItem* mapaItem) {
	std::cout << "Se descubre todo el mapa" << endl; 
	this->getPosicion(&mapa)->deleteEntidad(mapaItem);
	std::vector<Tile> tiles = mapa.allTiles();
	for(auto it = tiles.begin(); it != tiles.end(); ++it){
		Tile* tileExplorado = mapa.getTile(it->getU(), it->getV());
		agregarTilesExplorados(tileExplorado);

		BitStream bs;
		bs << PROTO::NIEBLA_SYNC << it->getU() << it->getV();
		sock.send(bs.str());
	}
	std::vector<Tile*> tileExplorados = tilesExplorados;	
	std::stringstream msj;
	msj << "Agarro Mapa";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	BitStream bs;
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());

}

void Personaje::aumentarVelocidad(char porcentaje) {
	if(this->velocidad == this->VELOCIDAD_DEFAULT){
		double aumento = porcentaje+100;
		this->velocidad*=(aumento/100);
	}
}

void Personaje::chocarConZapatos(Zapatos* zapatos) {
	std::cout << "Aumento la velocidad un " << (int)zapatos->getAumentoVelocidad() << "%\n";
	this->getPosicion(&mapa)->deleteEntidad(zapatos);
	this->aumentarVelocidad(zapatos->getAumentoVelocidad());
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::VEL << (float)pjm.getPjeLocal().getVelocidad();
	sock.send(bs.str());
	std::stringstream msj;
	msj << "Agarro Zapatos";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
}

void Personaje::chocarConBolaDeCristal(BolaDeCristal* bolaDeCristal) {

	this->getPosicion(&mapa)->deleteEntidad(bolaDeCristal);
	if(this->magia >= MAGIA_HECHIZO){
		std::cout << "veo niebla de los enemigos \n";

		BitStream bs;
		this->magia-=this->MAGIA_HECHIZO;
		bs << PROTO::UPDATE_ATT << ATT::MAGIA << this->getMagia();
		sock.send(bs.str());

		this->bolaDeCristal = true;
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::BOLA_DE_CRISTAL << true;
		sock.send(bs.str());

		//Aviso al server que agarro item para que lo desaparezca
		int x,y;
		x = this->getPosicion(&mapa)->getU();
		y = this->getPosicion(&mapa)->getV();
		bs.clear();
		bs << PROTO::ITEM_OFF << x << y;
		sock.send(bs.str());

		std::stringstream msj;
		msj << "Agarro Bola de cristal";
		consola.log(msj.str());
	}else{
		std::cout << "magia insuficiente para bola de cristal \n";	
	}
}

void Personaje::chocarConGolem(GolemItem* golem) {

	this->getPosicion(&mapa)->deleteEntidad(golem);
	this->golem = true;
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::GOLEM << true;
	sock.send(bs.str());
	std::stringstream msj;
	msj << "Agarro Golem";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
}

void Personaje::chocarConTerremoto(Terremoto* terremoto) {
	this->getPosicion(&mapa)->deleteEntidad(terremoto);
	this->terremoto++;
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::CANT_TERREMOTO << this->getTerremoto();
	sock.send(bs.str());
	std::stringstream msj;
	msj << "Agarro Terremoto";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
}

void Personaje::chocarConHielo(Hielo* hielo) {
	this->getPosicion(&mapa)->deleteEntidad(hielo);
	this->hielo++;
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::CANT_HIELO << this->getHielo();
	sock.send(bs.str());
	std::stringstream msj;
	msj << "Agarro Hielo";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
}

void Personaje::utilizarTerremoto(Mapa* mapa, PjeManager* pjm, ClientSocket* sock) {
	int radio=this->RADIO_HECHIZO;
	char dañoRealizado;
	Tile* tilePersonaje;
	int xPersonaje,yPersonaje;
	if ((this->tieneTerremoto()) && (this->getMagia()>=this->MAGIA_HECHIZO)) {
		std::cout << "Uso terremoto\n";
		BitStream bs;
		bs << PROTO::USE_ITEM << this->nickname << ITEM::TERREMOTO;
		sock->send(bs.str());
		this->magia-=this->MAGIA_HECHIZO;
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::MAGIA << this->getMagia();
		sock->send(bs.str());
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
						it->second.dañar(dañoRealizado);
						std::cout << "Se danio a " << it->first << " con terremoto, total de " << (int)dañoRealizado << endl;
						bs.clear();
						bs << PROTO::DAMAGE << this->getNick() << it->first << dañoRealizado;
						sock->send(bs.str());
					}
				}
			}
		}
		this->terremoto--; 
	}
}

void Personaje::utilizarHielo(Mapa* mapa, PjeManager* pjm) {
	int radio=this->RADIO_HECHIZO;
	Tile* tilePersonaje;
	int xPersonaje,yPersonaje;
	if ((this->tieneHielo()) && (this->magia>=this->MAGIA_HECHIZO)) {
		std::cout << "Uso hechizo hielo\n";
		BitStream bs;
		bs << PROTO::USE_ITEM << this->nickname << ITEM::HIELO;
		sock.send(bs.str());
		this->magia-=this->MAGIA_HECHIZO;
		bs.clear();
		bs << PROTO::UPDATE_ATT << ATT::MAGIA << this->getMagia();
		sock.send(bs.str());
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
						//Lo agrego a la lista de congelados
						nicks_congelados.push_back(it->first);
						tHielo.start();
						std::cout << "Se congelo a " << it->first << endl;
						bs.clear();
						bs << PROTO::CONGELAR << this->getNick() << it->first;
						sock.send(bs.str());
					}
				}
			}
		}
		this->hielo--;
	}
}

void Personaje::updateHielo() {
	if (tHielo.isStarted() && tHielo.getTicks()>Personaje::TIEMPO_DESCONGELAR) {
		std::cout << "Pidiendo al server que descongele a los que congele \n";
		//Descongelo los que habia hechizado
		for (auto it = nicks_congelados.begin(); it != nicks_congelados.end(); it++) {
			std::string nick = (*it);
			std::cout << "Descongelando a " << nick << endl;
			pjm.getPje(nick).freezar(false);
			BitStream bs;
			bs << PROTO::DESCONGELAR << nick;
			sock.send(bs.str());
		}
		tHielo.stop();
	}
}

 void Personaje::chocarConCorazon(Corazon* corazon) {
	this->getPosicion(&mapa)->deleteEntidad(corazon);
	this->energia+=corazon->getEnergiaGanada();
	if (this->energia>this->ENERGIA_TOTAL)
		this->energia=this->ENERGIA_TOTAL;
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::ENERGIA << pjm.getPjeLocal().getEnergia();
	sock.send(bs.str());
	std::stringstream msj;
	msj << "Agarro Corazon";
	consola.log(msj.str());

 	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
}

 void Personaje::chocarConBotella(Botella* botella) {
	 this->getPosicion(&mapa)->deleteEntidad(botella);
	 this->magia+=botella->getMagiaGanada();
	 if (this->magia>this->MAGIA_TOTAL)
		 this->magia=this->MAGIA_TOTAL;
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::MAGIA << pjm.getPjeLocal().getMagia();
	sock.send(bs.str());
	std::stringstream msj;
	msj << "Agarro Flechas";
	consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
 }

 void Personaje::chocarConFlechas(Flechas* flechas) {
	 this->getPosicion(&mapa)->deleteEntidad(flechas);
	 this->flechas+=flechas->getCantFlechas();
 }
 /*
 void Personaje::chocarConBombas(Bombas* bombas) {
	 this->getPosicion(&mapa)->deleteEntidad(bombas);
	 this->bombas+=bombas->getCantBombas();
 }*/

 void Personaje::chocarConGranadas(Granadas* granadas) {
	 this->getPosicion(&mapa)->deleteEntidad(granadas);
	 this->granadas+=granadas->getCantGranadas();
 }

 //Podria no recibir la varita, lo dejamos por si es necesario en un futuro
 void Personaje::chocarConVaritas(Varitas* varitas) {
	 this->getPosicion(&mapa)->deleteEntidad(varitas);
	 this->varita=true;
 }

 void Personaje::chocarConEscudo(Escudo* escudo) {
	 this->getPosicion(&mapa)->deleteEntidad(escudo);
	 this->energiaEscudo+=escudo->getEnergiaEscudo();
	 BitStream bs;
	 bs << PROTO::UPDATE_ATT << ATT::ENERGIA_ESCUDO << pjm.getPjeLocal().getEnergiaEscudo();
	 sock.send(bs.str());
	 std::stringstream msj;
     msj << "Agarro Escudo";
 	 consola.log(msj.str());

	//Aviso al server que agarro item para que lo desaparezca
	int x,y;
	x = this->getPosicion(&mapa)->getU();
	y = this->getPosicion(&mapa)->getV();
	bs.clear();
	bs << PROTO::ITEM_OFF << x << y;
	sock.send(bs.str());
 }

 void Personaje::chocarConBandera(Bandera* bandera) {
	 BitStream bs;
	 Tile* tilePersonaje = pjm.getPjeLocal().getPosicion(&mapa);
	 int x = tilePersonaje->getU();
	 int y = tilePersonaje->getV();
	 std::cout << "Atrape bandera en pos (" << x << "," << y << ")" << endl;
	 bs << PROTO::CATCH_FLAG << x << y;
	 sock.send(bs.str());
     std::stringstream msj;
 	 msj << "Atrapo Bandera";
 	 consola.log(msj.str());
 }

void Personaje::aumentarRadio(float proporcion) {
	radioY*=(1+proporcion);
	radioX=2*radioY;

	std::cout << "RADIO AUMENTADO: " << radioY << "\n";
	//notifico al servidor
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::RADIO << radioY;
	sock.send(bs.str());
}

void Personaje::muere() {
	bool murioLocal = false;
	if (this->getNick() == pjm.getPjeLocal().getNick()) murioLocal = true;
	if (vivo) {
		soundman.playSound("death", this->getX(), this->getY());
		this->animacionMuerte();
		std::cout << "Fui asesinado" << endl;
		//Redirecciono a la posicion inicial nuevamente
		this->vivo=false;
		//Aviso al server que me mori
		BitStream bs;
		bs << PROTO::DEAD << this->getNick();
		sock.send(bs.str());
		this->timerRevivir.start();
	}
 }

 void Personaje::revivir() {
	 this->vivo=true;
	 this->animacionRevivir();
	 this->energia=this->ENERGIA_TOTAL;
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
void Personaje::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
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
			if (!this->color || entidadDesconectada) {
				// bliteo del personaje
				this -> surf -> blitGris(dest , posX ,posY);	
			} else {
				this -> surf -> blit(dest , posX ,posY);		
			}
			// bliteo de la espada
			if (this -> getArmaActiva() != NULL) {
				this -> getArmaActiva() -> blit(dest,camara,mapa,posX,posY,this -> color);
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
void Personaje::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY , 
					bool color){	
	if ( (this -> imagen != NULL) && (this -> surf != NULL) &&
		(camara != NULL) ) {
		bool colorAux = this -> color;
		if ( (this ->highInTiles == 1) && (this -> widthInTiles == 1) ){
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
			if (!colorAux || entidadDesconectada) {
				this -> surf -> blitGris(dest , posX ,posY);	
			} else {
				this -> surf -> blit(dest , posX ,posY);
			}
			if(estaVivo() && !entidadDesconectada) {
				bar.setPerc(this->getEnergia());
				bar.blit(dest, posX+24, posY-8);
			}
			resman.getFont()->blitCentered(dest, posX+46, posY+9, this->getNick(), COLOR::WHITE);
			// bliteo de la espada
			if (this -> getArmaActiva() != NULL) {
				this -> getArmaActiva() -> blit(dest,camara,mapa,tileX,tileY,colorAux);
			}
		}
	}
}

Arma* Personaje::getArmaActiva(){
	return this -> espada;
}

std::pair<int,int> Personaje::buscarUbicacionBomba(int x, int y) {
	std::pair<int,int> pos;
	if (mapa.tileExists(x+1,y+1) && mapa.getTile(x+1,y+1)->isCaminable()) {
		pos = make_pair<int,int>(x+1, y+1);
	} else if (mapa.tileExists(x-1,y-1) && mapa.getTile(x-1,y-1)->isCaminable()) {
		pos = make_pair<int,int>(x-1, y-1);
	} else if (mapa.tileExists(x,y-1) && mapa.getTile(x,y-1)->isCaminable()) {
		pos = make_pair<int,int>(x, y-1);
	} else if (mapa.tileExists(x,y+1) && mapa.getTile(x-1,y+1)->isCaminable()) {
		pos = make_pair<int,int>(x, y+1);
	} else if (mapa.tileExists(x-1,y) && mapa.getTile(x-1,y)->isCaminable()) {
		pos = make_pair<int,int>(x-1, y);
	} else if (mapa.tileExists(x+1,y) && mapa.getTile(x+1,y)->isCaminable()) {
		pos = make_pair<int,int>(x+1, y);
	}
	return pos;
}

void Personaje::utilizarBomba(int xPersonaje, int yPersonaje) {
	this->bombas--;
	BitStream bs;
	bs << PROTO::UPDATE_ATT << ATT::CANT_BOMBAS << this->getCantBombas();
	sock.send(bs.str());
	std::pair<int,int> posBomba = this->buscarUbicacionBomba(xPersonaje,yPersonaje);
	int xBomba = posBomba.first;
	int yBomba = posBomba.second;
	this->posBombaX = xBomba;
	this->posBombaY = yBomba;
	//Agrego la bomba al mapa en el tile adyacente
	std::cout << "Colocando bomba en pos (" << xBomba << "," << yBomba << ")" << endl;
	ArmaBomba* bomba;
	bomba = new ArmaBomba("bomba",1,1,true, xBomba , yBomba,NULL,&mapa,resman,Imagen::COLOR_KEY );
	mapa.getTile(xBomba, yBomba)->addEntidad(bomba,&mapa);
	entidades_cargadas.push_back(bomba);
	this->setBombaColocada(bomba);
	//Aviso al server que puse bomba
	bs.clear();
	bs << PROTO::USE_ITEM << this->getNick() << ITEM::BOMBA << posBombaX << posBombaY;
	sock.send(bs.str());
	tBomba.start();
}
void Personaje::utilizarGolem() {
	BitStream bs;
	this->magia-=this->MAGIA_HECHIZO;
	bs.clear();
	bs << PROTO::UPDATE_ATT << ATT::MAGIA << this->getMagia();
	sock.send(bs.str());
	bs.clear();
	Tile* tileJugador = mapa.getTilePorPixeles(this->getX(),this->getY());
	bs << PROTO::USO_GOLEM << this->getNick() << tileJugador->getU() << tileJugador->getV();
	sock.send(bs.str());
}

void Personaje::updateBomba() {
	if ((this->tBomba.isStarted()) && (this->tBomba.getTicks()>3000)) {
		std::cout << "Explota la bomba! \n";
		//Daño a todo el radio
		int radio = 2;
		Tile* tilePersonaje;
		int xPersonaje,yPersonaje,dañoRealizado;
		for (int i=posBombaX-radio;i<=posBombaX+radio;i++) {
			for (int j=posBombaY-radio;j<=posBombaY+radio;j++) {
				//Veo si yo estoy en el radio
				tilePersonaje=pjm.getPjeLocal().getPosicion(&mapa);
				xPersonaje=tilePersonaje->getU();
				yPersonaje=tilePersonaje->getV();
				if ((i==xPersonaje) && (j==yPersonaje)) {
					srand (time(NULL));
					dañoRealizado=rand()%(this->ENERGIA_TOTAL+1);
					pjm.getPjeLocal().dañar(dañoRealizado);
					std::cout << "Me autodanie con bomba, total de " << (int)dañoRealizado << endl;
					BitStream bs;
					bs << PROTO::UPDATE_ATT << ATT::ENERGIA << this->getEnergia();
					sock.send(bs.str());
				}
				//Veo si algun personaje se encuentra en el radio
				for (auto it=pjm.getPjes().begin();it!=pjm.getPjes().end();it++) {
					tilePersonaje=it->second.getPosicion(&mapa);
					xPersonaje=tilePersonaje->getU();
					yPersonaje=tilePersonaje->getV();
					if ((i==xPersonaje) && (j==yPersonaje)) {
						srand (time(NULL));
						dañoRealizado=rand()%(this->ENERGIA_TOTAL+1);
						it->second.dañar(dañoRealizado);
						std::cout << "Se danio a " << it->first << " con bomba, total de " << (int)dañoRealizado << endl;
						BitStream bs;
						bs << PROTO::DAMAGE << this->getNick() << it->first << dañoRealizado;
						sock.send(bs.str());
					}
				}
			}
		}
		this->tBomba.stop();
		//Elimino la bomba del mapa
		mapa.getTile(posBombaX,posBombaY)->deleteEntidad(this->getBombaColocada());
		//Aviso al server que exploto la bomba
		BitStream bs;
		bs << PROTO::BOMB_OFF;
		sock.send(bs.str());
		std::cout << "Aviso a server que exploto mi bomba" << endl;
	}
}

void Personaje::utilizarTransmutacion(std::string nick_enemigo) {
	std::cout << "Uso hechizo de transmutacion con " << nick_enemigo << endl;
	//auto enemigo = pjm.getPje(nick_enemigo);
	//Veo en que se convierte, si en otro enemigo o en una lapida
	srand (time(NULL));
	char cambio=rand()%2;
	if (cambio == 0) {
		//Se convierte en lapida
		std::cout << "Convertiendo en piedra a " << nick_enemigo << endl;
		pjm.getPje(nick_enemigo).animacionPiedra();
		BitStream bs;
		bs << PROTO::TRANSMUT << TIPO::LAPIDA << nick_enemigo;
		sock.send(bs.str());
	} else {
		//Se convierte en un enemigo con distinta estrategia
		std::cout << "Convertido en otro tipo de enemigo" << endl;
		BitStream bs;
		bs << PROTO::TRANSMUT << TIPO::ESTRATEGIA_ENEMY << nick_enemigo;
		sock.send(bs.str());
	}
	tipoTransmut = cambio;
	transmutado = nick_enemigo;
	tTransmut.start();
}

void Personaje::updateTransmutacion() {
	if (tTransmut.isStarted() && tTransmut.getTicks()>Personaje::TIEMPO_TRANSMUT) {
		std::cout << "Volviendo atras la transmutacion \n";
		pjm.getPje(transmutado).animacionRevivir();
		BitStream bs;
		bs << PROTO::DESTRANSMUT << tipoTransmut << transmutado;
		sock.send(bs.str());
		tTransmut.stop();
	}
}

void Personaje::cambiarApariencia(const std::string& name,ResMan& rm , const int colorKey){
	if(this -> imagen != NULL) {
		ImagenPersonaje* imagenPersonaje = static_cast<ImagenPersonaje*>(this->imagen);
		unsigned int fps = imagenPersonaje -> getFPS() ;
		unsigned int delay = imagenPersonaje -> getDelay();
		unsigned int altoSprite = imagenPersonaje -> getAlto();
		unsigned int anchoSprite = imagenPersonaje -> getAncho();
		unsigned int accionActual = imagenPersonaje->getAccionActual();
		unsigned int accionSiguiente = imagenPersonaje ->getAccionSiguiente();
		delete(this -> imagen);
		this -> imagen	= new ImagenPersonaje(name.c_str() , altoSprite , 
								anchoSprite , fps , delay , rm ,colorKey);	
		imagenPersonaje = static_cast<ImagenPersonaje*>(this->imagen);
		imagenPersonaje -> setAccionActual(accionActual);
		imagenPersonaje -> setAccionSiguiente(accionSiguiente);
	}
}
