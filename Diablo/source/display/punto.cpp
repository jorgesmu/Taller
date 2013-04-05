#include "punto.h"

Punto::Punto(int x, int y){
	this -> x = x;
	this -> y = y;
}

int Punto::getX(){
	return x;
}

int Punto::getY(){
	return y;
}