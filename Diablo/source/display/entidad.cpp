#include "entidad.h"

	/*
		Pre:-

		Post: Inicializa los atributos con los valores por defecto.
	*/
	void Entidad::inicializarAtributosEnValoresDefault() {
		//posicion
		this -> offsetTileX = 0;
		this -> offsetTileY = 0;
		this -> posTileX = 0;
		this -> posTileY = 0;
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
					const unsigned int wTiles , const unsigned int hTiles ,
					int pixel_ref_x ,int pixel_ref_y,
					int posTileX, int posTileY , ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , pixel_ref_x , pixel_ref_y, posTileX, posTileY , rm , colorKey);
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad::Entidad(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int fps , const int delay , 
					int pixel_ref_x ,int pixel_ref_y,
					int posTileX, int posTileY , ResMan& rm , const int colorKey){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//carga de imagen y configuración inicial.
		init(name , wTiles , hTiles , fps , delay , pixel_ref_x , pixel_ref_y, 
			posTileX , posTileY , rm , colorKey);
	}
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	void Entidad::init(const std::string& name, 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int fps , const int delay , 
					int pixel_ref_x ,int pixel_ref_y,
					int posTileX, int posTileY , ResMan& rm , const int colorKey) {
		// Se destruyen imagenes previas
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		// Se setean los atributos a sus valores por defecto.
		this -> inicializarAtributosEnValoresDefault();
		// Se carga la nueva imagen
		this -> imagen	= new ImagenAnimada(name.c_str() ,Imagen::ALTO_DEFAULT , 
								Imagen::ANCHO_DEFAULT , fps , delay , rm ,colorKey);
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
	}

	/*
		Pre: La instancia ha sido creada.

		Post: Se ha configurado la instancia de acuerdo a a los parametros.

		NOTA: ImagenEstatica
	*/
	void Entidad::init(const std::string& name,  
					const unsigned int wTiles , const unsigned int hTiles , 
					int pixel_ref_x ,int pixel_ref_y , 
					int posTileX, int posTileY , 
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
		//posicion en Tiles
		this -> posTileX = posTileX;
		this -> posTileY = posTileY;
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
		this -> surf = NULL;
	}

	// Cambia la posicion de la entidad
	void Entidad::mover(const unsigned int x , const unsigned int y , 
						const unsigned int altoTileEnPixeles , const unsigned int anchoTileEnPixeles) {
		/*
		int posicionEnMapaX = this -> offsetTileX + anchoTileEnPixeles * this -> posTileX + this -> pixel_ref_x;
		int posicionEnMapaY = this -> offsetTileY + altoTileEnPixeles * this -> posTileY + this -> pixel_ref_y;
		*/

		//Por ahora lo muevo de Tile en Tile

		this ->posTileX = x;
		this ->posTileY = y;
	}
	
	/*
		Retorna la posición X en Tiles
	*/
	int Entidad::getPosTileX() const {
		return this -> posTileX;
	}
	
	/*
		Retorna la posición Y en Tiles
	*/
	int Entidad::getPosTileY() const {
		return this -> posTileY;
	}
			
	/*
		retorna el alto en tiles
	*/
	int Entidad::getHighInTiles() {
		return this -> highInTiles;
	}

	/*
		retorna el ancho en tiles
	*/
	int Entidad::getWidthInTiles() {
		return this -> widthInTiles;
	}

	// Actualiza las cosas internas, si hubiese
	void Entidad::update() {
		this -> surf = this -> imagen -> getSurface();
	}
	
	// Dibuja la entidad
	void Entidad::blit(SDL_Surface* dest, const Camara& camara , 
					const unsigned int tileX , 
					const unsigned int tileY){
		if ( (this -> imagen != NULL) && (this -> surf != NULL)) {
			if(this -> surf -> getSDL_Surface() != NULL){
				this ->surf -> blit(dest , tileX - (int)camara.getX(), tileY - (int)camara.getY());		
			}
		}

		/*
		int posicionEnMapaX = this -> offsetTileX + anchoTileEnPixeles * this -> posTileX - 
										this -> pixel_ref_x;
				int posicionEnMapaY = this -> offsetTileY + altoTileEnPixeles * this -> posTileY - 
										this -> pixel_ref_y;
				(this -> surf) -> blit(dest , posicionEnMapaX - (int) camara.getX(),
												posicionEnMapaY - (int) camara.getY());
												*/
	}