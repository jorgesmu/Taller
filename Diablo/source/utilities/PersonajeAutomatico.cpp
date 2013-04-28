#include "PersonajeAutomatico.h"
#include "../display/mapa.h"

/*
	Pre:-
		 
	Post: Se ha creado la instancia con los valores por defecto.
*/
PersonajeAutomatico::PersonajeAutomatico():Personaje(){
}

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los par?etros.

*/
PersonajeAutomatico::PersonajeAutomatico(const std::string& name, 
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

*/
void PersonajeAutomatico::init(const std::string& name, 
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
	//seteo tiempo proximo tiempo de actualizacion direccion
	this->tiempoProximoUpdateDireccion = clock() + 
								PersonajeAutomatico::DELTA_TIEMPO_UPDATE_DIRECCION;
}

// Actualiza las cosas internas, si hubiese
void PersonajeAutomatico::update(Mapa* mapa) {
	if (this -> tiempoProximoUpdate <= clock()){
		if (this -> tileDestino != NULL) {
			//actualizacion de posicion
			this -> actualizarPosicion(mapa);
		} else {
			if(this -> imagen != NULL) {
				this -> surf = this -> imagen -> getSurface();
			}
		}
		this -> tiempoProximoUpdate = clock() + this -> deltaUpdatePosicion;
	} 
	if (this -> tiempoProximoUpdateDireccion <= clock()){
		int value = rand()%10;
		int posX;
		int posY;
		if(value > 5){
			posX = this -> posX + (rand()%20) * Tile::TILE_ANCHO;
			value = rand()%10;
			if(value > 5){
				posY = this -> posY + (rand()%20) * Tile::TILE_ALTO;
			} else {
				posY = this -> posY - (rand()%20) * Tile::TILE_ALTO;
			}
		} else {
			posX = this -> posX - (rand()%20) * Tile::TILE_ANCHO;
			value = rand()%10;
			if(value > 5){
				posY = this -> posY + (rand()%20) * Tile::TILE_ALTO;
			} else {
				posY = this -> posY - (rand()%20) * Tile::TILE_ALTO;
			}
		}
		Tile* tile = mapa -> getTilePorPixeles(posX , posY);
		if (tile != NULL) {
			this -> mover(tile);
			this -> tiempoProximoUpdateDireccion = clock() + 
									PersonajeAutomatico::DELTA_TIEMPO_UPDATE_DIRECCION;
		}
	}
}