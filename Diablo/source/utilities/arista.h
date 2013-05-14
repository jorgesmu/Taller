#pragma once
#include <algorithm>
using namespace std;
class arista{
private:
	pair<int,int> destino;
	double  peso;	
public:
	//Constructor
	arista(int xDestino,int yDestino,double peso);

	//Getters
	int get_x_destino(); 

	int get_y_destino(); 

	double get_peso(); 

};