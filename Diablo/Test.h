#pragma once
#include "ImagenEstatica.h"
#include "ImagenAnimada.h"
class Test
{
private:
	
	Test(void);
	
	~Test(void);

public:

	//Todos los tests
	static bool test();

	//Test Imagen Estatica
	static bool testImagenEstatica();

	//Test Imagen Animada
	static bool testImagenAnimada();

	//Test Surface
	static bool testSurface();
};

