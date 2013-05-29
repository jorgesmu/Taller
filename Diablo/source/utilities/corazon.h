#pragma	once

#include "item.h"

class Corazon: public Item {
	private:
		int energiaGanada;
	public:
		//Constructor
		Corazon();

		Corazon(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles , bool caminable,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey):Item(name,wTiles,hTiles,caminable,pixel_ref_x,pixel_ref_y,tile,mapa,rm,colorKey) {}

		void chocarCon(Personaje* personaje) {
			personaje->chocarConCorazon(this);
		}

		int getEnergiaGanada() { return this->energiaGanada; }

};