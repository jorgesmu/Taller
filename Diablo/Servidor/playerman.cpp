#include "playerman.h"
#include "../source/utilities/aux_func.h"
#include "enemigoServer.h"
#include <exception>
#include <cassert>
#include <ctime>
#include <set>

Player::Player() {
	nick = "uninit-nick-pm";
	tipo_personaje = "uninit-pje";
	isOnline = false;
	x = y = 0;
	velocidad = 105/1000;
	congelado = 0; //descongelado
	xSiguiente = ySiguiente = -1;
	bolaDeCristal = false;
	seMovio = false;
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
int Player::getXSiguiente() const {
	return xSiguiente;
}

int Player::getYSiguiente() const {
	return ySiguiente;
}
std::string Player::getTipo() const {
	return tipo_personaje;
}
void Player::setPosSiguiente( int XNuevo,int YNuevo)  {
	xSiguiente = XNuevo;
	ySiguiente = YNuevo;
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

bool Player::existsTileRecorrido(short x, short y) {
	// ToDo: Esto se puede mejorar usando sort+binary search
	auto p = std::make_pair(x, y);
	if(std::find(tiles_recorridos.begin(), tiles_recorridos.end(), p) == tiles_recorridos.end()) {
		return false;
	}
	return true;
}

TilesRecorridos& Player::getTilesRecorridos() {
	return tiles_recorridos;
}

//Cosas de misiones
void Player::atrapoBandera(int x, int y) {
	banderasAtrapadas.push_back(std::make_pair(x,y));
}

int Player::cantBanderas() {
	return banderasAtrapadas.size();
}

bool Player::tieneBandera(int x, int y) {
	bool found = false;
	for (auto it = banderasAtrapadas.begin(); it != banderasAtrapadas.end(); it++) {
		if (it->first == x && it->second == y) {
			found = true;
			break;
		}
	}
	return found;
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
		//p.setPos(rand_x, rand_y);
		p.setPos(10,10);

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
//metodos de enemigos
// Devuelve si un player existe
bool PlayerManager::enemyExists(const std::string& nick) const {
	return enemy_map.find(nick) != enemy_map.end();
}

// Agrega un jugador
void PlayerManager::addEnemy(const std::string& nick, const std::string& tipo_pje, MapaServidor& mapa,int estrategiaElegida) {
	// Chequeo de sanidad
	assert(!enemyExists(nick));
	Enemigo*& p = enemy_map[nick];
	p = new Enemigo();
	p->init_Enemy(nick, tipo_pje,estrategiaElegida); 
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
		p->setOnline();
		p->setPos(rand_x, rand_y);
		//cargo pos
		p->setPosSiguiente(rand_x,rand_y);
	}
}

// Devuelva un jugador
Enemigo* PlayerManager::getEnemy(const std::string& nick) {
	assert(enemyExists(nick));
	return enemy_map[nick];
}

EnemyMapT& PlayerManager::getEnemies() {
	return this->enemy_map;
}