#include <cassert>
#include <iostream>
#include <sstream>

#include "soundman.h"
#include "../display/camara.h"
#include "../utilities/personaje.h"
#include "../utilities/aux_func.h"
#include "../utilities/logErrores.h"

// Ctor
SoundMan::SoundMan() {

}

// Dtor
SoundMan::~SoundMan() {
	//Se podria agregar un clean al destructor para evitar la assertion
	//this -> clean();
	// Checkeo que se hayan liberado todas las surfaces
	assert(sound_map.size() == 0);
}

bool SoundMan::init(Camara* cam, Personaje* pjl) {
	// Cargamos SDL_mixer con los siguientes formatos
	int flags = MIX_INIT_OGG|MIX_INIT_MP3;
	int initted = Mix_Init(flags);
	if(initted&flags != flags) {
		std::cerr << "Mix_Init() fail: " << Mix_GetError() << "\n";
		return false;
	}
	// Abrimos el canal de audio
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
		std::cerr << "Mix_OpenAudio() fail:" << Mix_GetError() << "\n";
		return false;
	}
	// Linkeamos a la camara
	cam_link = cam;
	MAX_AUDIBLE_DISTANCE = std::min(cam_link->getW(), cam_link->getH());
	std::cout << "MAX_AUDIBLE_DISTANCE: " << MAX_AUDIBLE_DISTANCE << "\n";
	// Linkeamos al personaje
	this->pjl = pjl;
	return true;
}

// Agrega el recurso con nombre name ubicado en el path dado por path
// Devuelve true o false y loggea en caso de error
bool SoundMan::addSound(const std::string& name, const std::string& path) {
	if(soundExists(name)) {
		std::cerr << "Error loading " << name << "; resource already exists\n";
		return false;
	}
	Mix_Chunk* m = Mix_LoadWAV(path.c_str());
	if(m == NULL) {
		std::cerr << "Error loading <" << name << ">: " << path << "\n";
		std::cerr << "Err: " << Mix_GetError() << "\n";
		return false;
	}else{
		sound_map[name] = m;
	}
	return true;
}

bool SoundMan::soundExists(const std::string& name) const {
	return sound_map.find(name) != sound_map.end();
}

// Devuelve un puntero a la Surface asociada con name
void SoundMan::playSound(const std::string& name, int source_x, int source_y) const {
	if(soundExists(name)) {
		Mix_Chunk* m = sound_map.find(name)->second;
		// Cambiamos el volumen de acuerdo a la distancia con el personaje
		int v = int(MIX_MAX_VOLUME - ( float(MIX_MAX_VOLUME) / float(this->MAX_AUDIBLE_DISTANCE) *
			::distance<float>(source_x, source_y, pjl->getX(), pjl->getY()) ));
		if(v < 0) v = 0;
		std::cout << "Source: (" << source_x << "," << source_y << ")\n";
		std::cout << "Listener: (" << pjl->getX() << "," << pjl->getY() << ")\n";
		std::cout << "Distance: " << ::distance<float>(source_x, source_y, pjl->getX(), pjl->getY()) << "\n";
		std::cout << "Sound volume: " << v << "\n";
		Mix_VolumeChunk(m, v);
		// Play
		Mix_PlayChannel(-1, m, 0);
	}else{
		std::stringstream ss;
		ss << "Error, invalid sound <" << name << "> requested\n";
		err_log.escribir(ss.str());
	}
}

void SoundMan::clean() {
	// Liberamos todos los chunks
	for(auto it = sound_map.begin();it != sound_map.end();it++) {
		Mix_FreeChunk(it->second);
	}
	// Borramos el mapa
	sound_map.clear();
	// Cerramos el canal de audio
	Mix_CloseAudio();
	// Force quit SDL_mixer
	while(Mix_Init(0)) Mix_Quit();
}