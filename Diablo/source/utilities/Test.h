#pragma once
#include "../utilities/ImagenEstatica.h"
#include "../utilities/ImagenAnimada.h"
#include "../utilities/ImagenPersonaje.h"
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

	
	//Test Imagen Personaje
	static bool testImagenPersonaje();

	//Test Entidad
	static bool testEntidad();

	//Test Surface
	static bool testSurface();
};

