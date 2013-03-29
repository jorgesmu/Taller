#pragma once

//No tiene en cuenta el scrolling, ver si ya se pasan las coordenadas de pantalla convertidas
//o las tengo que convertir yo. Es solo aplicar el desplazamiento del margen scroll.
class coordenadas  {

private:
	int h,w,fx,fy;
	int* tile;
	int* screen_coord;

public:
	/* Constructor
	Recibe el alto y ancho de un tile.
	Ademas las coordenadas (x,y) punto origen del mapa.
	Se define el origen como el vertice superior del tile logico (0,0).
	*/
	coordenadas(int alto_tile, int ancho_tile, int origen_x, int origen_y);

	//Destructor
	~coordenadas(void);

	//Devuelve un array con las coordenadas de pantalla del centro de un tile logico especificado como parametro
	int* coordenadas_pantalla(int tile_x, int tile_y);

	//Dadas las coordenadas en pantalla, devuelve al tile logica asociado a esa posicion
	int* coordenadas_tile(int coord_x, int coord_y);

};