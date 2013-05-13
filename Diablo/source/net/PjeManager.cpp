#include "PjeManager.h"
#include <cassert>

void PjeManager::addPje(const std::string& nick) {
	map_pjes[nick] = Personaje();
}

bool PjeManager::PjeExiste(const std::string& nick) const {
	return map_pjes.find(nick) != map_pjes.end();
}

Personaje& PjeManager::getPje(const std::string& nick) {
	assert(PjeExiste(nick));
	return map_pjes[nick];
}

Personaje& PjeManager::getPjeLocal() {
	return pje_local;
}

std::map<std::string, Personaje>& PjeManager::getPjes() {
	return map_pjes;
}