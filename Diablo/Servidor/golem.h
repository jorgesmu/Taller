#pragma once
#include "enemigoServer.h"

class Golem:public Enemigo{
	private:
		string duenio;
	public:
	Golem(string nickDuenio);
	bool personaje_en_radio(MapaServidor& mapa,PlayerManager& pm);

};