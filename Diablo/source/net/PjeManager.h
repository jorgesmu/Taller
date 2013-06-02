#pragma once

#include <map>
#include <string>

#include "../utilities/Personaje.h"
#include "../utilities/enemigo.h"
class PjeManager {
	private:
		std::map<std::string, Personaje> map_pjes;
		std::map<std::string, Enemigo> map_Enemigo;
		Personaje pje_local;
		Enemigo enemigo_local;

	public:
		void addPje(const std::string& nick);
		bool PjeExiste(const std::string& nick) const;
		Personaje& getPje(const std::string& nick);
		Personaje& getPjeLocal();
		std::map<std::string, Personaje>& getPjes();
		Enemigo& PjeManager::getEnemigoLocal() ;
		void addEnemigo(const std::string& nick);
		bool EnemigoExiste(const std::string& nick) const;
		Enemigo& getEnemigo(const std::string& nick);
		std::map<std::string, Enemigo>& getEnemigos();
};