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

//pjemanager para enemigos
void PjeManager::addEnemigo(const std::string& nick) {
	map_Enemigo[nick] = Enemigo();
}

bool PjeManager::EnemigoExiste(const std::string& nick) const {
	return map_Enemigo.find(nick) != map_Enemigo.end();
}

Enemigo& PjeManager::getEnemigo(const std::string& nick) {
	assert(EnemigoExiste(nick));
	return map_Enemigo[nick];
}


std::map<std::string, Enemigo>& PjeManager::getEnemigos() {
	return map_Enemigo;
}

Enemigo& PjeManager::getEnemigoLocal() {
	return enemigo_local;
}