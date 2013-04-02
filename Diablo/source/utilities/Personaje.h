#pragma once
#include "../display/entidad.h"

class Personaje : public Entidad {

public:
	
	const static int VELOCIDAD_DEFAULT = 5;

private:

	unsigned int velocidad;

public:

	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	Personaje();
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los parametros.
	*/
	Personaje(const std::string& name, const std::string& path , 
			const unsigned int wTiles , const unsigned int hTiles , 
			const int fps , const int delay , const unsigned int velocidad ,
			const int pixel_ref_x , const int pixel_ref_y,
			const int posTileX , const int posTileY , ResMan& rm , 
			const int colorKey);

	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.
	*/
	virtual void init(const std::string& name, const std::string& path , 
					const unsigned int wTiles , const unsigned int hTiles , 
					const int fps , const int delay , const unsigned int velocidad ,
					const int pixel_ref_x , const int pixel_ref_y,
					const int posTileX , const int posTileY , ResMan& rm , 
					const int colorKey);

	/*
		Pre: La instancia ha sido creada.
		 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	virtual ~Personaje(void);

};