//#include "entidadFija.h"
//#include "entidad.h"
//#include "tile.h"
//#include "mapa.h"
//
//
//EntidadFija::EntidadFija(const std::string& name, 
//			const unsigned int wTiles , const unsigned int hTiles , 
//			const unsigned int fps , const unsigned int delay , 
//			const unsigned int altoSprite , const unsigned int anchoSprite ,
//			const unsigned int velocidad , 
//			const int pixel_ref_x , const int pixel_ref_y,
//			Tile* tile , 
//			ResMan& rm , const int colorKey){
//	// seteo del puntero a imagen
//	this -> imagen = NULL;
//	//carga de imagen y configuración inicial.
//	init(name , wTiles , hTiles , fps , delay , 
//		altoSprite , anchoSprite ,
//		velocidad,
//		pixel_ref_x , pixel_ref_y, 
//		tile , rm , colorKey);
//}
//
//void EntidadFija::init(const std::string& name, 
//		const unsigned int wTiles , const unsigned int hTiles , 
//		const unsigned int fps , const unsigned int delay , 
//		const unsigned int  altoSprite , const unsigned int anchoSprite ,
//		const unsigned int velocidad ,
//		int pixel_ref_x ,int pixel_ref_y,
//		Tile* tile, 
//		ResMan& rm , const int colorKey) {
//	// Se destruyen imagenes previas
//	if (this -> imagen != NULL) {
//		delete(this -> imagen);
//		this -> imagen = NULL;
//	}
//	// Se setean los atributos a sus valores por defecto.
//	this -> inicializarAtributosEnValoresDefault();
//	// Se carga la nueva imagen
//	this -> imagen	= new ImagenAnimada(name.c_str() , altoSprite , 
//							anchoSprite, rm ,colorKey);
//	// Seteo de nombre 
//	this -> name = name;
//	// Seteo del Surface
//	this -> surf = this -> imagen -> getSurface();
//	//Seteo de dimensiones
//	this -> highInTiles = hTiles;
//	this -> widthInTiles = wTiles;
//	//pixel de referencia
//	this -> pixel_ref_x = pixel_ref_x ;
//	this -> pixel_ref_y = pixel_ref_y;
//	//tile destino
//	this -> tileDestino = tile;
//	//tile ancla
//	this -> tileAncla = tile;
//	//deltaUpdatePosicion
//	this -> deltaUpdatePosicion = Entidad::BASE_DE_TIEMPO/velocidad;
//	this -> velocidad = velocidad;
//	//tiempo siguiente update
//	this -> tiempoProximoUpdate = clock();
//	//agrego entidad al tile
//	if (tile != NULL) {
//		if(this -> tileAncla != NULL){
//			this -> tileAncla -> deleteEntidad(this);
//		}
//		this -> tileAncla = tile;
//		this -> tileAncla -> addEntidad(this);
//		//seteo posicion
//		this -> posX = tile -> getX();
//		this -> posY = tile -> getY();
//	}
//	//seteo como compartido
//	this -> compartido = false;
//}
//
///*
//	Pre:-
//		 
//	Post: Se ha inicializado la instancia segun el archivo y los par?etros.
//
//*/
//void EntidadFija::init(const std::string& name, 
//		const unsigned int wTiles , const unsigned int hTiles , 
//		const unsigned int fps , const unsigned int delay , 
//		const unsigned int  altoSprite , const unsigned int anchoSprite ,
//		const unsigned int velocidad ,
//		int pixel_ref_x ,int pixel_ref_y,
//		Tile* tile, 
//		ResMan& rm , const int colorKey) {
//	// Se destruyen imagenes previas
//	if (this -> imagen != NULL) {
//		delete(this -> imagen);
//		this -> imagen = NULL;
//	}
//	// Se setean los atributos a sus valores por defecto.
//	this -> inicializarAtributosEnValoresDefault();
//	// Se carga la nueva imagen
//	this -> imagen	= new ImagenPersonaje(name.c_str() , altoSprite , 
//							anchoSprite , fps , delay , rm ,colorKey);
//	// Seteo de nombre 
//	this -> name = name;
//	// Seteo del Surface
//	this -> surf = this -> imagen -> getSurface();
//	//Seteo de dimensiones
//	this -> highInTiles = hTiles;
//	this -> widthInTiles = wTiles;
//	//pixel de referencia
//	this -> pixel_ref_x = pixel_ref_x ;
//	this -> pixel_ref_y = pixel_ref_y;
//	//tile destino
//	this -> tileDestino = tile;
//	//tile ancla
//	this -> tileAncla = tile;
//	//deltaUpdatePosicion
//	this -> deltaUpdatePosicion = Entidad::BASE_DE_TIEMPO/velocidad;
//	this -> velocidad = velocidad;
//	//tiempo siguiente update
//	this -> tiempoProximoUpdate = clock();
//	//agrego entidad al tile
//	if (tile != NULL) {
//		if(this -> tileAncla != NULL){
//			this -> tileAncla -> deleteEntidad(this);
//		}
//		this -> tileAncla = tile;
//		this -> tileAncla -> addEntidad(this);
//		//seteo posicion
//		this -> posX = tile -> getX();
//		this -> posY = tile -> getY();
//	}
//	//seteo como compartido
//	this -> compartido = false;
//}
//
//void EntidadFija::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
//						const unsigned int tileX ,	const unsigned int tileY, Tile* tile){
//		if ( (this -> imagen != NULL) && (this -> surf != NULL) &&
//			(camara != NULL) ) {
//				if(tile == tileAncla){
//					posX = this -> posX - (int)(camara -> getX()) - this -> pixel_ref_x;
//					posY = this -> posY - (int)(camara -> getY()) - this -> pixel_ref_y;
//				
//					this -> surf -> blit(dest , posX ,posY);		
//				}
//		}
//}