#include "Personaje.h"

/*
	Pre:-
		 
	Post: Se ha creado la instancia con los valores por defecto.
*/
Personaje::Personaje():Entidad(){
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

*/
void Personaje::init(const std::string& name, 
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
	Pre: Los parámetros respetan la siguiente convención:

	"x" e "y": Coordenadas del Tile destino

	Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

	Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
	en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
		
	Nota2: Los destinos no validos no traeran problemas al algoritmo, es decir que
	le podes pasar cualquier destino aunque supere las dimensiones del mapa.
*/
void Personaje::mover(const unsigned int x , const unsigned int y) {
	this -> posTileDestinoX = x;
	this -> posTileDestinoY = y;
	unsigned int direccion = this -> calcularDireccion();
	if (direccion != CENTRO) {
			
	}
}
