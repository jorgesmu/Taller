#include "arista.h"
using namespace std;


arista::arista(int xDestino,int yDestino,double pesoArista){
	destino = make_pair<int,int>(xDestino,yDestino);
	peso = pesoArista;
}

//Getters
int arista::get_x_destino(){
	return destino.first;
} 

int arista::get_y_destino(){
	return destino.second;
} 

double arista::get_peso(){
	return peso;
}
