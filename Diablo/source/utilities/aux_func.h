#ifndef AUX_FUNC_H
#define AUX_FUNC_H

#include "SDL.h"

// Dos funciones para simplificar la creacion de SDL_Rect
// La primera deja height y widght undefined
SDL_Rect makeRect(int x, int y);
SDL_Rect makeRect(int x, int y, int w, int h);

#endif