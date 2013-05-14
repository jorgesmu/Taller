#pragma once
#include <vector>
#include "arista.h"
using namespace std;
class vertice {
private:

	vector<arista> aristas;
	bool visitado;
	int x,y;
public:
	vertice();

	vertice(int pos_x,int pos_y);
	//setters
	void agregar_arista(int posX,int posY, double peso);
	
	//marca el vertice como visitado
	void visitar();
	
	//getters
	vector<arista> get_aristas();

	int get_x();

	int get_y();
	
	bool get_visitado();
	//setter
	
	void set_x(int newValue);

	void set_y(int newValue);
	
	//otros
	//marca un vertice como no visitado
	void reinciar();
};