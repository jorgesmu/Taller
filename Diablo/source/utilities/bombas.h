#pragma	once

#include "arma.h"

class Bombas: public Arma {
	
	private:
	
		int cantBombas;
	
	public:
	
		//Constructor
		Bombas();
/*
		Bombas(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles , bool caminable,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey):Arma(name,wTiles,hTiles,caminable,pixel_ref_x,pixel_ref_y,tile,mapa,rm,colorKey) {}
			*/
		void chocarCon(Personaje* personaje) {
			personaje->chocarConBombas(this);
		}

		int getCantBombas() { return this->cantBombas; }

};