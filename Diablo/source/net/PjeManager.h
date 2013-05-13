#pragma once

#include <map>
#include <string>

#include "../utilities/Personaje.h"

class PjeManager {
	private:
		std::map<std::string, Personaje> map_pjes;
		Personaje pje_local;

	public:
		void addPje(const std::string& nick);
		bool PjeExiste(const std::string& nick) const;
		Personaje& getPje(const std::string& nick);
		Personaje& getPjeLocal();
		std::map<std::string, Personaje>& getPjes();

};