#include "playerman.h"
#include "../source/utilities/aux_func.h"

#include <exception>
#include <cassert>
#include <ctime>
#include <set>

Player::Player() {
	nick = "uninit-nick-pm";
	tipo_personaje = "uninit-pje";
	isOnline = false;
	x = y = 0;
	velocidad = 0;
	congelado = 0; //descongelado
}

Player::~Player() {
	// Nada
}

void Player::init(const std::string& nick, const std::string& pje) {
	this->nick = nick;
	this->tipo_personaje = pje;
}

void Player::setPos(int x, int y) {
	this->x = x;
	this->y = y;
}

int Player::getX() const {
	return x;
}

int Player::getY() const {
	return y;
}

std::string Player::getTipo() const {
	return tipo_personaje;
}

std::string Player::getNick() const {
	return nick;
}

bool Player::isOn() const {
	return this->isOnline;
}

void Player::setOffline() {
	isOnline = false;
}

void Player::setOnline() {
	isOnline = true;
}

void Player::addTileRecorrido(short x, short y) {
	// ToDo: Esto se puede mejorar usando sort+binary search
	auto p = std::make_pair(x, y);
	if(std::find(tiles_recorridos.begin(), tiles_recorridos.end(), p) == tiles_recorridos.end()) {
		this->tiles_recorridos.push_back(p);
	}
}

TilesRecorridos& Player::getTilesRecorridos() {
	return tiles_recorridos;
}

//////////////


// Devuelve si un player existe
bool PlayerManager::playerExists(const std::string& nick) const {
	return player_map.find(nick) != player_map.end();
}

// Agrega un jugador
void PlayerManager::addPlayer(const std::string& nick, const std::string& tipo_pje, MapaServidor& mapa) {
	// Chequeo de sanidad
	assert(!playerExists(nick));
	Player& p = player_map[nick];
	p.init(nick, tipo_pje);
	// Buscar posicion en el mapa
	bool found = false;
	int x = 0; // Contador de iteraciones
	int w, h; // Para guardar el tamaño del mapa
	mapa.getSize(&w, &h);
	//std::cout << "Mapa size: " << w << "," << h << "\n";
	int rand_x, rand_y;
	// Limitamos la cantidad de iteraciones
	while(!found && x < 1000) {
		rand_x = intRand(0, w-1);
		rand_y = intRand(0, h-1);
		//std::cout << rand_x << "," << rand_y << "\n";
		if(mapa.getTile(rand_x, rand_y)->isCaminable()) {
			found = true;
		}
		x++;
	}
	if(!found) {
		throw std::runtime_error("No se pudo ubicar el personaje\n");
	}else{
		p.setOnline();
		p.setPos(rand_x, rand_y);
	}
}

// Devuelva un jugador
Player& PlayerManager::getPlayer(const std::string& nick) {
	assert(playerExists(nick));
	return player_map[nick];
}

PlayerMapT& PlayerManager::getPlayers() {
	return this->player_map;
}