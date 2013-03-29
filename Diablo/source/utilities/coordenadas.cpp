#include "coordenadas.h"

/* Constructor
Recibe el alto y ancho de un tile.
Ademas las coordenadas (x,y) punto origen del mapa.
Se define el origen como el vertice superior del tile logico (0,0).
*/
coordenadas::coordenadas(int alto_tile, int ancho_tile, int origen_x, int origen_y) {
	h=alto_tile/2;
	w=ancho_tile/2;
	fx=origen_x;
	fy=origen_y;
	tile=new int[2];
	screen_coord=new int[2];
}

//Destructor
coordenadas::~coordenadas(void) {
	delete [] tile;
	delete [] screen_coord;
}


//Devuelve un array con las coordenadas de pantalla del centro de un tile logico especificado como parametro
int* coordenadas::coordenadas_pantalla(int tile_x, int tile_y) {
	screen_coord[0]=tile_x*w-tile_y*w+fx;
	screen_coord[1]=tile_x*h+tile_y*h+fy;
	return screen_coord;
}

//Dadas las coordenadas en pantalla, devuelve al tile logica asociado a esa posicion
int* coordenadas::coordenadas_tile(int coord_x, int coord_y) {
	tile[0]=coord_x*h+coord_y*w-fx*h-fy*w;
	tile[0]/=2*h*w;
	tile[1]=-coord_x*h+coord_y*w+fx*h-fy*w;
	tile[1]/=2*h*w;
	return tile;
}