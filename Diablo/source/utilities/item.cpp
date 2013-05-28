#include "item.h"

Item::Item() {
}
/*
Item::Item(const std::string& name,
			const unsigned int wTiles , const unsigned int hTiles , bool caminable,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , Mapa* mapa,
			ResMan& rm , const int colorKey) {
	::EntidadFija(name,wTiles,hTiles,caminable,pixel_ref_x,pixel_ref_y,tile,mapa,rm,colorKey);
}
*/
void Item::chocarConPersonaje(Personaje* personaje) {
	std::cout << "Agarro item\n";
}