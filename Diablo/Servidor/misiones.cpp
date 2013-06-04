#include "misiones.h"
#include "../source/utilities/aux_func.h"

extern MapaServidor mapa;

void Misiones::agregarBandera() {
	bool done=false;
	int cont=0;
	int x,y; //posiciones de la bandera
	int w,h; //dimensiones del mapa
	mapa.getSize(&w, &h);
	while (!done && cont<999) {
		x=intRand(0,w-1);
		y=intRand(0,h-1);
		if(mapa.getTile(x, y)->isCaminable()) {
			done = true;
		}
		cont++;
	}
	//Agrego la nueva bandera a la lista
	banderas.push_back(std::make_pair(x,y));
}

void Misiones::agregarMultiplesBanderas(int n) {
	for (int i=0;i<n;i++) {
		agregarBandera();
	}
}

bool Misiones::hayBandera(int x, int y) {
	bool found = false;
	for (auto it = banderas.begin(); it != banderas.end(); it++) {
		if (it->first == x && it->second == y) {
			found = true;
			break;
		}
	}
	return found;
}

void Misiones::crearMisionBanderas(int n) {
	this->setTipo(this->MISION_BANDERAS);
	this->agregarMultiplesBanderas(n);
}

int Misiones::cantBanderas() {
	return banderas.size();
}
