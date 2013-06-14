#pragma once

#include <string>
#include <vector>
#include <list>
#include "../source/net/defines.h"

class ItemServidor {
	private:
	unsigned char tipo;
	int posX;
	int posY;

	public:
	ItemServidor();
	~ItemServidor();
	void init(const unsigned char tipo, int x, int y);
	unsigned char getTipo();
	int getX() const;
	int getY() const;
};