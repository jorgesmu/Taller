#include "Personaje.h"

/*
	Pre:-
		 
	Post: Se ha creado la instancia con los valores por defecto.
*/
Personaje::Personaje(){
		// seteo del puntero a imagen
		this -> imagen = NULL;
		//seteo de velocidad
		this -> velocidad = Personaje::VELOCIDAD_DEFAULT;
		// Se setean los atributos a sus valores por defecto.
		Entidad::inicializarAtributosEnValoresDefault();
}

/*
	Pre:-
		 
	Post: Se ha inicializado la instancia segun el archivo y los parametros.
*/
Personaje::Personaje(const std::string& name, const std::string& path , 
			const unsigned int wTiles , const unsigned int hTiles , 
			const int fps , const int delay , const unsigned int velocidad ,
			const int pixel_ref_x , const int pixel_ref_y,
			const int posTileX , const int posTileY , ResMan& rm , 
			const int colorKey) {
	// seteo del puntero a imagen
	this -> imagen = NULL;
	//carga de imagen y configuración inicial.
	this -> init(name , path , wTiles , hTiles , fps , delay , velocidad , pixel_ref_x , pixel_ref_y, 
		posTileX , posTileY , rm , colorKey);
}

/*
	Pre: Se ha creado la instancia.

	Post: Se ha seteado la instancia de acuerdo a los parametros.

	NOTA: ImagenAnimada
*/
void Personaje::init(const std::string& name, const std::string& path , 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int fps , const int delay , const unsigned int velocidad ,
					const int pixel_ref_x , const int pixel_ref_y,
					const int posTileX , const int posTileY , ResMan& rm , const int colorKey) {
	this -> velocidad = velocidad;
	//Se iniciliza a valores default padre
	Entidad::init(name, wTiles , hTiles , fps , delay , pixel_ref_x , pixel_ref_y, 
			posTileX , posTileY , rm , colorKey);
}

/*
	Pre: La instancia ha sido creada.
		 
	Post: Se ha destruido la instancia liberando los recursos asociados.
*/
Personaje::~Personaje(void) {
	if (this -> imagen != NULL) {
		delete(this -> imagen);
		this -> imagen = NULL;
	}
	this -> surf = NULL;
}
