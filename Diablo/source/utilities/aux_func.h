#pragma once

#include <cstdlib>
#include "SDL.h"

// Dos funciones para simplificar la creacion de SDL_Rect
// La primera deja height y widght undefined
SDL_Rect makeRect(int x, int y);
SDL_Rect makeRect(int x, int y, int w, int h);

// Genera un entero aleatorio entre min y max incluidos
int intRand(int min, int max);
// Idem pero float
float fRand(float v1, float v2);