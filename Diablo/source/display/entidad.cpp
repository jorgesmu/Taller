#include "entidad.h"

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos x, y, w y h en cero, surf e 
		imagenPersonaje en NULL, y name y path como cadenas vac?s.
	*/
	Entidad::Entidad(){
		this -> h = 0;
		this -> w = 0;
		this -> x = 0;
		this -> y = 0;
		this -> name = "";
		this -> path = "";
		this -> surf = NULL;
		this -> imagen = NULL;
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad::Entidad(const std::string& name, const std::string& path , const unsigned int w , 
					const unsigned int h , const int fps , const int delay) {
		this -> imagen	= new ImagenAnimada(path.c_str() , this -> h , this -> w , fps , 
														delay , Imagen::COLOR_KEY);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

		NOTA: ImagenAnimada
	*/
	Entidad::Entidad(const std::string& name, const std::string& path , const unsigned int w , 
					const unsigned int h , const int fps , const int delay , const int colorKey) {
		this -> imagen	= new ImagenAnimada(path.c_str() , this -> h , this -> w , fps , delay , colorKey);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:

		Post:

		NOTA: ImagenAnimada
	*/
	void Entidad::init(const std::string& name , const std::string& path , const unsigned int w , 
						const unsigned int h , const int fps , const int delay){
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> imagen	= new ImagenAnimada(path.c_str() , this -> h , this -> w , fps , 
														delay , Imagen::COLOR_KEY);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:

		Post:

		NOTA: ImagenAnimada
	*/
	void Entidad::init(const std::string& name , const std::string& path , const unsigned int w , 
						const unsigned int h , const int fps , const int delay , const int colorKey){
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> imagen	= new ImagenAnimada(path.c_str() , this -> h , this -> w , fps , 
														delay , Imagen::COLOR_KEY);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:

		Post:

		NOTA: ImagenEstatica
	*/
	void Entidad::init(const std::string& name , const std::string& path){
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> imagen	= new ImagenEstatica(path.c_str() , Imagen::COLOR_KEY);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:

		Post:

		NOTA: ImagenEstatica
	*/
	void Entidad::init(const std::string& name , const std::string& path , const int colorKey){
		if (this -> imagen != NULL) {
			delete(this -> imagen);
			this -> imagen = NULL;
		}
		this -> imagen	= new ImagenEstatica(path.c_str() , colorKey);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:

		Post:

		NOTA: ImagenEstatica
	*/
	Entidad::Entidad(const std::string& name , const std::string& path ){
		this -> imagen	= new ImagenEstatica(path.c_str());
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
	}

	/*
		Pre:

		Post:

		NOTA: ImagenEstatica
	*/
	Entidad::Entidad(const std::string& name , const std::string& path , const int colorKey){
		this -> imagen	= new ImagenEstatica(path.c_str() , colorKey);
		this -> name = name;
		if ( this -> imagen -> getPath() != NULL){
			this -> path = this -> imagen -> getPath();
		}
		this -> x = 0;
		this -> y = 0;
		this -> w = this -> imagen -> getAncho(); 
		this -> h = this -> imagen -> getAlto();
		this -> surf = this -> imagen -> getSurface();
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
	void Entidad::mover(int x, int y) {
		this -> x = x;
		this -> y = y;
	}
	
	// Getters para la posicion
	int Entidad::getX() const {
		return this -> x;
	}
	int Entidad::getY() const{
		return this -> y;
	}
	
	// Actualiza las cosas internas, si hubiese
	void Entidad::update() {
		this -> surf = this -> imagen -> getSurface();
	}
	
	// Dibuja la entidad
	void Entidad::blit(SDL_Surface* dest, SDL_Rect& cam){
		if ( (this -> imagen != NULL) && (this -> surf != NULL)) {
			if(this -> surf -> getSDL_Surface() != NULL){
				(this -> surf) -> blit(dest , this -> x , this -> y , cam);
			}
		}
	}

	int Entidad::getH(){
		return h;
	}

	int Entidad::getW(){
		return w;
	}