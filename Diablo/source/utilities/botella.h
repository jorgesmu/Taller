#pragma	once

#include "item.h"

class Botella: public Item {
	private:
		char magiaGanada;
	public:
		//Constructor
		Botella();

		Botella(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles , bool caminable,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey):Item(name,wTiles,hTiles,caminable,pixel_ref_x,pixel_ref_y,tile,mapa,rm,colorKey) {
		magiaGanada = 50;
		}

		void chocarCon(Personaje* personaje) {
			personaje->chocarConBotella(this);
		}

		char getMagiaGanada() { return this->magiaGanada; }

};