#include "Test.h"
	
	//Todos los tests
	bool Test::test() {
		bool retorno = true;
		retorno = testImagenEstatica()&& retorno;
		return retorno;
	}

	//Test Imagen Estatica
	bool Test::testImagenEstatica() {
		ImagenEstatica("../resources/arbol.bmp");
		return true;
	}