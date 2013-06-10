#pragma once
#include "playerman.h"
#include "mapaservidor.h"


class Enemigo:public Player{
	private:
		unsigned int estrategia,direccion;
		int pasosCaminados;

	public:
		unsigned int lineaRecta;
		void init_Enemy(const std::string& nickname,const std::string& tipo,unsigned int estrategiaElegida);
		//getters
		TileServidor* Enemigo::get_proximo_tile_recta_horizontal(MapaServidor& Mapa);
		TileServidor* Enemigo::get_proximo_tile_recta_vertical(MapaServidor& Mapa);
		TileServidor* Enemigo::get_proximo_tile_circular(MapaServidor& Mapa);
		TileServidor* get_proximo_tile_enemigo(MapaServidor& Mapa,PlayerManager& pm);
		virtual bool personaje_en_radio(MapaServidor& mapa,PlayerManager& pm, TileServidor*& tilePersonajes);

};