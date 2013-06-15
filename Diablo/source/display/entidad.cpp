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
	this -> posY = 0;
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
	//seteo posicion
	this -> posX = 0;
	this -> posY = 0;
	//seteo como compartido
	this -> compartido = true;
	this -> caminable = true;
	this -> no_dibujar_fuera_flag = false;
	if ((this -> highInTiles == 1) && (this -> widthInTiles == 1)) {
		this -> ordenBliteo = Entidad::ORDEN_PISO;
	} else{
		this -> ordenBliteo = Entidad::ORDEN_EDIFICIO;	
	}
	this->esItem = false;
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
				const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
				int pixel_ref_x ,int pixel_ref_y,
				Tile* tile,
				ResMan& rm , const int colorKey){
	// seteo del puntero a imagen
	this -> imagen = NULL;
	//carga de imagen y configuración inicial.
	init(name , wTiles , hTiles ,caminable, pixel_ref_x , pixel_ref_y, tile , rm , colorKey);
}

/*
	Pre:-
	 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.
	NOTA: ImagenAnimada
*/
Entidad::Entidad(const std::string& name, 
				const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
				const unsigned int fps , const unsigned int delay , 
				int pixel_ref_x ,int pixel_ref_y,
				Tile* tile , 
				ResMan& rm , const int colorKey){
	// seteo del puntero a imagen
	this -> imagen = NULL;
	//carga de imagen y configuración inicial.
	init(name , wTiles , hTiles ,caminable , fps , delay ,
		Imagen::ALTO_DEFAULT , Imagen::ANCHO_DEFAULT ,
		pixel_ref_x , pixel_ref_y, 
		tile , rm , colorKey);
}

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.

	NOTA: ImagenAnimada
*/
Entidad::Entidad(const std::string& name, 
		const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
		const unsigned int fps , const unsigned int delay , 
		const unsigned int altoSprite , const unsigned int anchoSprite ,
		const int pixel_ref_x , const int pixel_ref_y,
		Tile* tile , 
		ResMan& rm , const int colorKey){
	// seteo del puntero a imagen
	this -> imagen = NULL;
	//carga de imagen y configuración inicial.
	init(name , wTiles , hTiles , caminable ,fps , delay , 
		altoSprite , anchoSprite ,
		pixel_ref_x , pixel_ref_y, 
		tile , rm , colorKey);
}

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.

	NOTA: ImagenAnimada
*/
void Entidad::init(const std::string& name, 
				const unsigned int wTiles , const unsigned int hTiles , bool caminable ,
				const unsigned int fps , const unsigned int delay , 
				const unsigned int  altoSprite , const unsigned int anchoSprite ,
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
	//tile ancla
	this -> tileAncla = tile;
	//seteo como compartido
	this -> compartido = true;
	this -> caminable = caminable;
	if ((this -> highInTiles == 1) && (this -> widthInTiles == 1)) {
		this -> ordenBliteo = Entidad::ORDEN_PISO;
	} else{
		this -> ordenBliteo = Entidad::ORDEN_EDIFICIO;	
	}
}

/*
	Pre: La instancia ha sido creada.

	Post: Se ha configurado la instancia de acuerdo a a los parametros.

	NOTA: ImagenEstatica
*/
void Entidad::init(const std::string& name,  
				const unsigned int wTiles , const unsigned int hTiles , bool caminable , 
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
	//tile ancla
	this -> tileAncla = tile;
	//seteo como compartido
	this -> compartido = true;
	this -> caminable = caminable;
	this -> entidadDesconectada = false;
	if ((this -> highInTiles == 1) && (this -> widthInTiles == 1)) {
		this -> ordenBliteo = Entidad::ORDEN_PISO;
	} else{
		this -> ordenBliteo = Entidad::ORDEN_EDIFICIO;	
	}
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
	if(this ->tileAncla != NULL){
		this ->tileAncla->deleteEntidad(this);
	}
	this -> surf = NULL;
	this -> tileAncla = NULL;
}

std::string Entidad::get_nombre(){
	return name;
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

	
float Entidad::getX(){
	return this -> posX;
}

float Entidad::getY(){
	return this -> posY;
}

bool Entidad::getDibujada(){
	return this->dibujada;
}

void Entidad::setDibujada(bool seDibujo, Mapa* mapa, Personaje* personaje){
	this->dibujada = seDibujo;
}


// Actualiza las cosas internas, si hubiese
unsigned int Entidad::update(Mapa* mapa) {
	if(this -> imagen != NULL) {
		this -> surf = this -> imagen -> getSurface();
	}
	return 0;
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
			if (!this->color || entidadDesconectada) {
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
void Entidad::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
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
		}
	}
}

/*	
	Pre;
*/
void Entidad::actualizarImagen(const unsigned int direccion){
	if(this -> imagen != NULL) {
		// Actualizacion del surface
		this -> surf = this -> imagen -> getSurface();
	}
}

/*
	Pre: Se ha creado la instancia.
	Post: Se setea el tile donde se encuentra la instancia.
*/
void Entidad::setTileActual(Tile* tile) {
	if (tile != NULL){
		if(this->tileAncla != tile) {
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
		}
		this -> tileAncla = tile;
	}
}

/*
	Pre: La instancia ha sido creada.
	
	Post:Se cambia el tile donde se encuentra la misma.
*/
void Entidad::setTileActual(Tile* tile , Mapa* mapa){
	this -> setTileActual(tile);
}
	
/*
	Pre: La instancia ha sido creada.
	Post: Se retorna verdadero si se puede ocupar el tile ocupado 
	por dicha instancia.
*/
bool Entidad::isCaminable(Tile* tile){
	return true;
}

// Deprecated
bool Entidad::isCaminable(){
	return true;
}
/*
	Pre: La instancia ha sido creada.
	Post: Se retorna el tile donde se encuentra la instancia.
*/
Tile* Entidad::getPosicion(Mapa* mapa){
	Tile* retorno=NULL;
	if (mapa != NULL) {
		retorno=mapa -> getTilePorPixeles(this -> posX , this -> posY);
	}
	return retorno;
}

std::vector<Tile*> Entidad::getTilesExplorados(){
	return tilesExplorados;
}

bool Entidad::agregarTilesExplorados(Tile* tile){
	if (std::find(tilesExplorados.begin(), tilesExplorados.end(), tile) == tilesExplorados.end()){
		this -> tilesExplorados.push_back(tile);
		return true;
	}
	return false;
}

// Deprecated
void Entidad::setColor(bool value){
	this -> color = value;
}


void Entidad::setColor(bool value , int tileX , int tileY, Mapa* mapa, int persX, int persY, Personaje* personaje) {
	if ( (this -> posX == tileX) && (this -> posY) == tileY) {
		this -> color = value;	
	}
}

bool Entidad::isInTile(int x , int y){
	return ((this -> posX == x) && (this -> posY == y));
}

bool Entidad::isCompartido(){
	return this -> compartido;
}

unsigned int Entidad::getOrdenBliteo(){
	return this -> ordenBliteo;
}

void Entidad::setOrdenBliteo(unsigned int orden){
	this -> ordenBliteo = orden;
}

void Entidad::chocarCon(Personaje* personaje) {
	personaje->chocarConEntidad();
}
