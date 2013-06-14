#include "itemservidor.h"

ItemServidor::ItemServidor() {
	this->posX = 0;
	this->posY = 0;
}

ItemServidor::~ItemServidor() {
	// Nada
}

void ItemServidor::init(unsigned char tipo, int x, int y) {
	this->tipo = tipo;
	this->posX = x;
	this->posY = y;
}

unsigned char ItemServidor::getTipo() {
	return tipo;
}

int ItemServidor::getX() const {
	return posX;
}

int ItemServidor::getY() const {
	return posY;
}
