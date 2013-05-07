#pragma once

#include <cstdlib>
#include "SDL.h"
#include <fstream>
#include <vector>
#include <string>

// Dos funciones para simplificar la creacion de SDL_Rect
// La primera deja height y widght undefined
SDL_Rect makeRect(int x, int y);
SDL_Rect makeRect(int x, int y, int w, int h);

// Genera un entero aleatorio entre min y max incluidos
int intRand(int min, int max);
// Idem pero float
float fRand(float v1, float v2);

// Devuelve el tamaño en bytes de un archivo o 0 en caso de error
size_t fileSize(const std::string& file);

// Devuelve un vector con todos los archivos en el directorio (en el argumento ret)
void listFilesinDir(std::string dir, std::vector<std::string>& ret);

// Constantes de colores generales
namespace COLOR {
	const SDL_Color RED = { 242, 0, 0 };
	const SDL_Color BLUE = { 84, 84, 255 };
	const SDL_Color GREEN = { 0, 234, 0};
	const SDL_Color YELLOW = { 128,128,0 };
	const SDL_Color WHITE = {255,255,255};
	const SDL_Color GRAY = {180,180,180};
}