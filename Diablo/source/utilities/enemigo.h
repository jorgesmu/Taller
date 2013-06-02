#pragma once
#include "Personaje.h"
#include "../display/tile.h"
#include "../display/mapa.h"
#include "../display/tile.h"
class PjeManager;
class Enemigo : public Personaje{
private:
	unsigned int personajeAAtacar,estrategia,direccion;
	int pasosCaminados;
public:
	unsigned int lineaRecta;
	void init_Enemy(const std::string& nickname,const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned int anchoSprite ,
			const unsigned int velocidad , 
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile , 
			ResMan& rm , const int colorKeyunsigned, int PersonajeDestino,unsigned int estrategiaElegida);
	//getters
	unsigned int get_personaje_a_atacar(); 
	Tile* Enemigo::get_proximo_tile_recta_horizontal(Mapa& Mapa);
	Tile* Enemigo::get_proximo_tile_recta_vertical(Mapa& Mapa);
	Tile* Enemigo::get_proximo_tile_circular(Mapa& Mapa);
	Tile* get_proximo_tile_enemigo(Mapa& Mapa,PjeManager& pjm);
	bool personaje_en_radio(Mapa& mapa,PjeManager& pjm);

};