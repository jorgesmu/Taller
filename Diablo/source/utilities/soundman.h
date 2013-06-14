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
	// Musica
	Mix_Music* music;
	bool music_muted;
	// Links
	Camara* cam_link;
	Personaje* pjl;
	// Maxima distancia en pixeles, donde el volumen desde la fuente se hace 0
	int MAX_AUDIBLE_DISTANCE;
	public:
	// Ctor
	SoundMan();
	// Dtor
	~SoundMan();
	// Carga el sound manager, linkeando a la camara y al personaje local
	// Carga la musica
	bool init(Camara* cam, Personaje* pjl);
	// Agrega un sonido <nombre_recurso, path>
	bool addSound(const std::string& name, const std::string& path);
	// Chequea si un recurso existe
	bool soundExists(const std::string& name) const;
	// Play de un sonido localizado: nombre de recurso + (x,y) en pixeles absolutos (respecto al 0,0 del mapa)
	void playSound(const std::string& name, int source_x, int source_y) const;
	// Play de un sonido no localizado (volumen constante)
	void playSound(const std::string& name) const;
	// Play de la musica
	void playMusic() const;
	// Stop musica
	void stopMusic() const;
	// Toggle de mute para musica
	void toggleMusicMute();
	// Libera todos los sonidos - se tiene que llamar antes de salir
	void clean();

};