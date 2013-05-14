#pragma once
#include <vector>
#include "vertice.h"

class grafo{
private:
	vector<vector<vertice> > vertices;
	vector<vector<pair<int,int> > >  padre;
	vector<vector<double> > distancia;
	vector < vector<bool> > visitado;
	int ancho,alto;
public:
	grafo(int tamAncho,int tamAlto);

	//setter
	//agrega una arista a un vertice de un grafo
	void agregar_arista (int pos_x_vertice, int pos_y_vertice,int pos_x_arista, int pos_y_arista,double peso_arista);
	//getters
	//devuelve los vectores
	vector<vector<vertice> > get_vertices();
	//devuelve un vertice 
	vertice get_vertice(int pos_x,int pos_y);
	//otros
	//devuelve el camino minimo entre un nodo inicial y uno final
	vector<pair<int,int> > camino(int posXOrigen,int posYOrigen, int posXDestino,int posYDestino);
private:
	vector<pair<int,int> > reconstruir_camino(int posXOrigen,int posYOrigen, int posXDestino,int posYDestino);

};
