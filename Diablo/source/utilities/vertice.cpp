#include "vertice.h"
#include "arista.h"

vertice::vertice(){}


vertice::vertice(int pos_x,int pos_y){
	visitado = false;
	x = pos_x;
	y = pos_y;
}
void vertice::agregar_arista(int posX,int posY, double peso){
	arista nuevaArista(posX,posY,peso);
	aristas.push_back(nuevaArista);
}

//marca el vertice como visitado
void vertice::visitar(){
	visitado = true;
}
//getters
int vertice::get_x(){
	return x;
}
//getters
int vertice::get_y(){
	return y;
}
vector<arista>& vertice::get_aristas(){
	return aristas;
}
void vertice::set_x(int newValue){
	x = newValue;
}

void vertice::set_y(int newValue){
	y = newValue;
}
	
bool vertice::get_visitado(){
	return visitado;
}

void vertice::reinciar(){
	visitado = true;
}