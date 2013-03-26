#include "SDL.h"
#include "timer.h"

Timer::Timer() {
    // Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start() {
    started = true;
    paused = false;
    startTicks = double(SDL_GetTicks());
}

void Timer::start(double ticks) {
    started = true;
    paused = false;
    sync(ticks);
}

void Timer::stop() {
    started = false;
    paused = false;
}

void Timer::pause() {
    if(started && !paused) {
        paused = true;
        pausedTicks = double(SDL_GetTicks()) - startTicks;
    }
}

void Timer::unpause() {
    if(paused) {
        paused = false;
        startTicks = double(SDL_GetTicks()) - pausedTicks;
        pausedTicks = 0;
    }
}

float Timer::getTicks() const {
    if(started) {
        if(paused) {
            return float(pausedTicks);
        }else{
            return float(double(SDL_GetTicks()) - startTicks);
        }
    }
    return 0;
}

bool Timer::isStarted() const {
    return started;
}

bool Timer::isPaused() const {
    return paused;
}

void Timer::sync(double ticks) {
    startTicks = SDL_GetTicks() - ticks;
}
