#include "entidadFija.h"
#include "tile.h"
#include "mapa.h"

	/*
		Pre:-
		 
		Post: Se ha creado la instancia con los valores por defecto.
	*/
	EntidadFija::EntidadFija(){
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
	EntidadFija::EntidadFija(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles ,
					int pixel_ref_x ,int pixel_ref_y,
					Tile* tile, Mapa* mapa,
					ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , pixel_ref_x , pixel_ref_y, tile, mapa , rm , colorKey);
	}
		
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	EntidadFija::EntidadFija(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned int anchoSprite ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay , 
			altoSprite , anchoSprite ,
			pixel_ref_x , pixel_ref_y, 
			tile, mapa, rm , colorKey);
	}
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	void EntidadFija::init(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , 
					const unsigned int fps , const unsigned int delay , 
					const unsigned int  altoSprite , const unsigned int anchoSprite ,
					int pixel_ref_x ,int pixel_ref_y,
					Tile* tile , Mapa* mapa,
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
		this -> tileAncla = NULL;
		// seteo el tile actual		
		if (tile != NULL) {
			//seteo posicion
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
			this -> agregarAnclas(mapa);
		}
	}

	/*
		Pre: La instancia ha sido creada.

		Post: Se ha configurado la instancia de acuerdo a a los parametros.

		NOTA: ImagenEstatica
	*/
	void EntidadFija::init(const std::string& name,  
					const unsigned int wTiles , const unsigned int hTiles , 
					int pixel_ref_x ,int pixel_ref_y , 
					Tile* tile , Mapa* mapa,
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
		this -> tileAncla = NULL;
		//seteo el tile actual
		if (tile != NULL) {
			//seteo posicion
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
			this -> agregarAnclas(mapa);
		}
	}
	
	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	EntidadFija::~EntidadFija() {
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> surf = NULL;
		this -> tileDestino = NULL;
		this -> tileAncla = NULL;
	}
	
	// --------------------------------------------------------------------------

	/*
		Se borrara despues
	*/
	void EntidadFija::mover(Tile* tileDestino) {
		
	}
		
	// --------------------------------------------------------------------------

	/*
		Pre: Los parámetros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el 

		Post: Se ha pintado la entidad en el surface dest según la camara y el mapa.
		Se pinta en la posicion actual de la imagen.

		NOTA: Se supone que se llama luego de acceder al tile donde fue anclada.
	*/
	void EntidadFija::blit(SDL_Surface* dest , Camara* camara , Mapa* mapa,
						const unsigned int tileX ,	const unsigned int tileY){
		if ( (this -> imagen != NULL) && (this -> surf != NULL) &&
			(camara != NULL) ) {
			if(this -> surf -> getSDL_Surface() != NULL){
				// Esto no andaba bien, lo "comente" y ahora funciona
				if( (this -> widthInTiles <= 1) && (this -> highInTiles <= 1) && false) {
					int posX;
					int posY;
					posX = (int)tileX - (int)(camara -> getX()) - this -> pixel_ref_x;
					posY = (int)tileY - (int)(camara -> getY()) - this -> pixel_ref_y;
					this -> surf -> blit(dest , tileX , tileY);	
				}
				else {
					if ( ((tileX == posX) && (tileY == posY)) || (this -> tileAncla == NULL) ){
						int posX = this -> posX - (int)(camara -> getX()) - this -> pixel_ref_x;
						int posY = this -> posY - (int)(camara -> getY()) - this -> pixel_ref_y;
						this -> surf -> blit(dest , posX , posY);
					} else{
						int posX;
						int posY = this -> posY - (int)(camara -> getY()) - this -> pixel_ref_y;
						int delta = (int)tileX - this -> tileAncla -> getX();
						SDL_Rect rect;					
						posX = this -> posX - (int)(camara -> getX()) - this -> pixel_ref_x + delta;
						rect.x = delta;
						rect.y = 0;
						rect.h = this -> imagen -> getAlto();
						rect.w = Tile::TILE_ANCHO;
						this -> surf -> blit(dest , posX , posY , rect);
					}
				}
			}
		}
	}
	
	/*
		Pre: La instancia ha sido creada.
		Post: Se han agregado las anclas correspondientes de acuerdo a la base.
	*/
	void EntidadFija::agregarAnclas(Mapa* mapa){
		int contTilesX = 0;
		int delta = 1;
		Tile* anclaPrincipal = mapa -> getTilePorPixeles(this -> posX , this -> posY);
		if (anclaPrincipal != NULL){
			bool anclaPrincipalHallada = false;
			this -> tileAncla = anclaPrincipal;
			while(contTilesX < (int)(this -> widthInTiles)){
				bool salida = false;
				int contTilesY = 0;
				Tile* ancla = NULL;
				while((contTilesY < (int)(this -> highInTiles)) && (!salida)){
					int posAnclaX = this -> posX + (contTilesX - contTilesY) * Tile::TILE_ANCHO/2;
					int posAnclaY = this -> posY + (contTilesY + contTilesX)*Tile::TILE_ALTO/2;
					Tile* ancla = mapa -> getTilePorPixeles(posAnclaX , posAnclaY);
					if (ancla != NULL){
						ancla -> addEntidad(this);
						contTilesY += delta;
						if(!anclaPrincipalHallada){
							this -> tileAncla = ancla;
						}
					} else {
						salida = true;
					}
				}
				anclaPrincipalHallada = true;
				contTilesX += delta;
			}
		}
	}

	//Deprecated
	void EntidadFija::setTileActual(Tile* tile){
		
	}

	
	/*
		Pre: La instancia ha sido creada.

		Post: Si la instancia no tiene definida sus anclas, setea la posicion y las anclas.
	*/
	void EntidadFija::setTileActual(Tile* tile , Mapa* mapa){
		//se puede anclar una sola vez
		if ( (tile != NULL) && (mapa != NULL) && (tileAncla == NULL)){
			this -> posX = tile -> getX();
			this -> posY = tile -> getY();
			this -> agregarAnclas(mapa);
		}
	}
	
	
	/*
		Actualiza el surface de la instancia.
	*/
	void EntidadFija::update(Mapa* mapa) {
		if (this->imagen != NULL){
			this -> surf = this -> imagen -> getSurface();
		}
	}