#pragma once

#include <string>
#include <map>
#include <array>
#include "SDL.h"
#include "SDL_mixer.h"
#include "aux_func.h"

class Personaje;
class Camara;

class SoundMan {
	private:
	std::map<std::string, Mix_Chunk*> sound_map; // Hash table de surfaces
	Camara* cam_link;
	Personaje* pjl;
	int MAX_AUDIBLE_DISTANCE;
	public:
	// Ctor
	SoundMan();
	// Dtor
	~SoundMan();
	// Carga la surface de error y los fonts
	bool init(Camara* cam, Personaje* pjl);
	bool addSound(const std::string& name, const std::string& path);
	bool soundExists(const std::string& name) const;
	void SoundMan::playSound(const std::string& name, int source_x, int source_y) const;
	// Libera todas las surfaces - se tiene que llamar antes de salir
	void clean();

};