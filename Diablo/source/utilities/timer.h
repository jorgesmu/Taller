#ifndef TIMER_H
#define TIMER_H

#include "SDL.h"

// Clase Timer
// Unidad de tiempo: 1 tick = 1 ms = 1/1000 seg

class Timer {

	private:
    double startTicks;
    double pausedTicks;
    bool paused;
    bool started;

	public:
	// Constructor default
    Timer();
	// Arrancar el timer desde 0
    void start();
	// Arrancar el timer con una cantidad de ticks ya predefinidos en vez de desde 0
    void start(double ticks);
	// Para el timer (resetea a 0)
    void stop();
	// Pausa el timer
    void pause();
	// Unpause
    void unpause();
	// Devuelve los ticks que pasaron
    float getTicks() const;

	// Settea el timer a una cierta cantidad de ticks
	// Util para syncronizar entre 2 computadoras, se usa mas adelante seguramente
    void sync(double ticks);

	// Funciones de query de estado
    bool isStarted() const;
    bool isPaused() const;

	// Funcion estatica (de clase) para sacar el tiempo desde que arranco el programa
	// Puede ser util para algo, se usa haciendo Timer::time()
    static float time() {
        return float(SDL_GetTicks());
    }

};

#endif
