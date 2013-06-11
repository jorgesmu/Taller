#pragma once
#include "enemigoServer.h"
#include <string>
using namespace std;
class Golem:public Enemigo{
	private:
		string duenio;
	public:
	Golem(const std::string& nickname,const std::string& tipo,unsigned int estrategiaElegida,string nickDuenio);
	virtual bool personaje_en_radio(MapaServidor& mapa,PlayerManager& pm,TileServidor*& tilePersonaje);
	string& get_duenio(){return duenio;}
	virtual bool personaje_adyacente(MapaServidor& mapa,PlayerManager& pm, TileServidor*& tilePersonaje,string& enemigoAtacado);
	virtual TileServidor* get_proximo_tile_enemigo(MapaServidor& mapa,PlayerManager& pm);

};