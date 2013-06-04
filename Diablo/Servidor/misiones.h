#include <list>
#include <utility>
#include "mapaservidor.h"

class Misiones{
	private:
		char tipo; //0=atrapar banderas, 1=matar enemigo
		std::list<std::pair<int,int>> banderas; //lista de banderas en mapa(solo interesan sus posiciones)

	public:
		//Constantes
		const static char MISION_BANDERAS = 0;
		const static char MISION_ENEMIGO = 1;
		//Agrega una nueva bandera al mapa en una posicion random
		void agregarBandera();

		//Agrega n nuevas banderas al mapa en posiciones random
		void agregarMultiplesBanderas(int n);

		//Verifico que haya una bandera en la posicion dada
		bool hayBandera(int x, int y);

		char getTipo() { return tipo; }

		void setTipo(char newTipo) { tipo=newTipo; }

		//Crea una nueva mision para atrapar banderas
		//n es la cantidad de banderas para atrapar
		void crearMisionBanderas(int n);

		//Cantidad de banderas a atrapar
		int cantBanderas();

};