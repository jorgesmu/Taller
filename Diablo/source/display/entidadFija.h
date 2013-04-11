//#pragma	once
//
//#include <string>
//#include "../utilities/surface.h"
//#include "../utilities/Imagen.h"
//#include "../utilities/ImagenEstatica.h"
//#include "../utilities/ImagenAnimada.h"
//#include "../display/camara.h"
//#include "../display/resman.h"
//#include "SDL.h"
//#include "entidad.h"
//class Mapa;    // Declaración adelantada de Mapa
//class Tile;    // Declaración adelantada de Tile
///*	
//	Aclaraciones sobre esta clase:
//
//	* Acerca de mover:
//	  ----------------
//	El movimiento de la entidad se fija con la función "mover" de manera tal que una vez 
//	especificado un "Tile Destino" la entidad fijara su curso hacia la misma y a medida que se 
//	pida la actualización de su estado mediante "update", la misma actualizara su posición
//	navegando por el mapa.
//	Un llamado al método mover puede implicar que la instancia cambie su posicion dentro del Tile
//	tomado como referencia, o que cambie de Tile.
//
//	* Acerca de Update:
//	  -----------------
//	Update se encarga de actualizar posiciones sobre el Tile y sobre el Mapa a la velocidad de la
//	entidad.
//
//	Varios llamdos a Update sin hacer un blit, puede implicar que se produzcan varios 
//	desplazamientos unitarios a la velocidad dada (algo así como pasos), de manera tal
//	que se produzca un desplazamiento poco natural sobre el mapa.
//
//	--> !!!! Importante: 
//			Hacer solo un update por entidad al momento de pintar. Esto es porque
//		si vas a pintar todos los tiles y haces un update por cada Tile pintado, puede pasar que
//		se te mueva la entidad y te aparezca cortada en varios Tiles.
//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	
//	* Acerca de blit:
//	  ---------------
//	Como una entidad puede ocupar uno o varios Tiles, y la aplicacion actualiza la pantalla de
//	acuerdo a un orden dado por los Tiles, al momento de llamar a blit se tendra que especificar
//	sobre que "Tile" pintara la entidad.
//
//*/
//class EntidadFija : public Entidad {
//
//	public:
//
//	void EntidadFija::init(const std::string& name, 
//		const unsigned int wTiles , const unsigned int hTiles , 
//		const unsigned int velocidad ,
//		int pixel_ref_x ,int pixel_ref_y,
//		Tile* tile,
//		ResMan& rm , const int colorKey) {
//
//	//Imagen Animada
//	EntidadFija(const std::string& name, 
//			const unsigned int wTiles , const unsigned int hTiles , 
//			const unsigned int fps , const unsigned int delay , 
//			const unsigned int altoSprite , const unsigned  int anchoSprite ,
//			const unsigned int velocidad ,
//			const int pixel_ref_x , const int pixel_ref_y,
//			Tile* tile, 
//			ResMan& rm , const int colorKey);
//
//	
///*
//		Pre: Se ha creado la instancia.
//
//		Post: Se ha seteado la instancia de acuerdo a los parametros.
//
//		NOTA: ImagenAnimada
//	*/
//	virtual void init(const std::string& name,
//					const unsigned int wTiles , const unsigned int hTiles , 
//					const unsigned int fps , const unsigned int delay ,
//					const unsigned int altoSprite , const unsigned int anchoSprite ,
//					const unsigned int velocidad ,
//					const int pixel_ref_x , const int pixel_ref_y,
//					Tile* tile, 
//					ResMan& rm , const int colorKey);
//		void blit(SDL_Surface* dest , Camara* camara , Mapa* mapa, const unsigned int tileX , const unsigned int tileY, Tile* tile);
//
//};