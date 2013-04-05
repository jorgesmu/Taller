#include "mapa.h"
#include "camara.h"
#include "punto.h"

// Ctor
Mapa::Mapa() {
	w = h = 0;
}

// Hace un resize del mapa, es indefinido que pasa si el mapa no estaba en size 0
void Mapa::resize(int w, int h) {
	this->w = w;
	this->h = h;
	tiles.resize(w*h);
	assignTileCoords();
}

// Getter para cada tile
//#include <iostream>
Tile& Mapa::getTile(int x, int y) {
	//std::cout << "Accessing tile (" << x << ";" << y << "): " << x*h + y << "\n";
	return tiles[x*h + y];
}

// Getter para el vector de tiles, para recorrerlo entero
TileVec& Mapa::allTiles() {
	return tiles;
}

Punto tileWalk(std::string direccion, Punto punto, int rejilla){
	if(direccion == "este"){
		Punto punto(punto.getX() + 1, punto.getY() - 1);
		return punto;
	}
	if(direccion == "oeste"){
		Punto punto(punto.getX() - 1, punto.getY() + 1);
		return punto;
	}
	if(direccion == "norte"){
		Punto punto(punto.getX() - 1, punto.getY() - 1);
		return punto;
	}
	if(direccion == "sur"){
		Punto punto(punto.getX() + 1, punto.getY() + 1);
		return punto;
	}
	if(direccion == "noreste"){
		Punto punto(punto.getX(), punto.getY() - 1);
		return punto;
	}
	if(direccion == "noroeste"){
		Punto punto(punto.getX() - 1, punto.getY());
		return punto;
	}
	if(direccion == "sureste"){
		Punto punto(punto.getX() + 1, punto.getY());
		return punto;
	}
	if(direccion == "suroeste"){
		Punto punto(punto.getX(), punto.getY() + 1);
		return punto;
	}
	if(direccion == "ninguna"){
		Punto punto(punto.getX(), punto.getY());
		return punto;
	}
}

Punto MouseMap (Punto punto, Camara camara){
	int x0 = punto.getX() - (Tile::TILE_ANCHO/2);
	int y0 = punto.getY();
	
	int xCasilla = y0 + (x0 / 2);
	int yCasilla = y0 - (x0 / 2);
	xCasilla /= Tile::TILE_ALTO;
	yCasilla /= Tile::TILE_ALTO;
	return Punto(xCasilla, yCasilla);
}

void Mapa::blit(SDL_Surface* dest, const Camara& camara) {

////Coordenadas de las esquinas
//   Punto supIzq(0,0), supDch(0,0), infIzq(0,0), infDch(0,0);
//   Punto rejilla(0,0);
//   //Variables para recorrer filas y casillas
//   Punto filaInicio(0,0), filaFin(0,0), filaActual(0,0);
//   int contadorFilas = 0;
//
//   //Centinelas
//   bool terminado, terminadoFila;
//
//   /***********************************************************************/
//   /* FASE 1 - Calculamos las coordenadas de las casillas en cada esquina */
//   /***********************************************************************/
//   //Esquina superior izquierda:
//   rejilla = MouseMap (Punto (camara.getX(), camara.getY()), camara);
//
//   supIzq = tileWalk ("este", Punto (0, 0), rejilla.getX());
//   supIzq = tileWalk ("sur", supIzq, rejilla.getY());
//
//   //Esquina superior derecha:
//   rejilla = MouseMap (Punto (camara.getX() + 300, camara.getY()), camara);//300 es el ancho de la camara
//
//   supDch = tileWalk ("este", Punto (0, 0), rejilla.getX());
//   supDch = tileWalk ("sur", supDch, rejilla.getY());
//
//   //Esquina inferior izquierda:
//   rejilla = MouseMap (Punto (camara.getX(), camara.getY() + 300), camara);//300 es el alyo de la camara
//
//   infIzq = tileWalk ("este", Punto (0, 0), rejilla.getX());
//   infIzq = tileWalk ("sur", infIzq, rejilla.getY());
//
//   //Esquina inferior derecha:
//   rejilla = MouseMap (Punto (camara.getX() + 300, camara.getY() + 300), camara);//300 de ancho primero y 300 de alto despues
//
//   infDch = tileWalk ("este", Punto (0, 0), rejilla.getX());
//   infDch = tileWalk ("sur", infDch, rejilla.getY());
//
//   /*******************************************/
//   /* FASE 2 - Ampliamos el marco de dibujado */
//   /*******************************************/
//
//   //Desplazamos cada esquina para alejarnos de la pantalla
//   supIzq = tileWalk ("noroeste", supIzq, 1);
//   supDch = tileWalk ("noreste", supDch, 1);
//   infIzq = tileWalk ("suroeste", infIzq, 1);
//   infDch = tileWalk ("sureste", infDch, 1);
//
//   //Desplazamos las esquinas inferiores 2 pasos al sur para
//   //compensar por los objetos altos
//   infIzq = tileWalk ("sur", infIzq, 2);
//   infDch = tileWalk ("sur", infDch, 2);
//
//   /******************************/
//   /* FASE 3 - Bucle de dibujado */
//   /******************************/
//
//   terminado = false;
//   filaInicio = supIzq;
//   filaFin = supDch;
//
//   //Para cada fila
//   while (!terminado) {
//      terminadoFila = false;
//      //Seleccionamos la primera casilla
//      filaActual = filaInicio;
//      //Para cada casilla
//      while (!terminadoFila) {
//         //Dibujamos la casilla
//         getTile(filaActual.getX(), filaActual.getY()).blit(dest, camara);
//
//         //Comprobamos si hemos llegado al final de la fila y si no,
//         //nos movemos a la siguiente casilla
//         if ((filaActual.getX() == filaFin.getX()) && (filaActual.getY() == filaFin.getY()))
//            terminadoFila = true;
//         else
//			 filaActual = tileWalk ("este", filaActual, 1);
//      }
//      //Comprobamos si la fila recorrida era la ultima
//      if (((filaInicio.getX() == infIzq.getX()) && (filaInicio.getY() == infIzq.getY())) && ((filaFin.getX() == infDch.getX()) && (filaFin.getY() == infDch.getY())))
//         terminado = true;
//      else {
//         //Si no lo era, movemos las casillas de inicio y fin
//         //hacia abajo para comenzar con la siguiente
//         if (contadorFilas & 1) {
//            //Fila impar
//            filaInicio = tileWalk ("suroeste", filaInicio, 1);
//            filaFin = tileWalk ("sureste", filaFin, 1);
//         } else {
//            //Fila par
//            filaInicio = tileWalk ("sureste", filaInicio, 1);
//            filaFin = tileWalk ("suroeste", filaFin, 1);
//         }
//         ++contadorFilas;
//      }
//   }

	//asi estaba antes!!   
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		it->blit(dest, camara);
	}
}

void Mapa::clean() {
	for(auto it = tiles.begin(); it != tiles.end(); ++it) {
		it->clean();
	}
}

void Mapa::assignTileCoords() {
	int x, y;
	for(x = 0;x < w;x++) {
		for(y = 0;y < h;y++) {
			getTile(x,y).setBlitCoords((x-y)*Tile::TILE_ANCHO/2, (x+y)*Tile::TILE_ALTO/2);
		}
	}
}