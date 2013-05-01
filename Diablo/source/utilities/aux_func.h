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

// Devuelve el tama�o en bytes de un archivo o 0 en caso de error
size_t fileSize(const std::string& file);

// Devuelve un vector con todos los archivos en el directorio (en el argumento ret)
void listFilesinDir(std::string dir, std::vector<std::string>& ret);