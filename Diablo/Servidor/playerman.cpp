#include "playerman.h"
#include "../source/utilities/aux_func.h"
#include "enemigoServer.h"
#include "golem.h"
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
	xSiguiente = ySiguiente = -1;
	bolaDeCristal = false;
	seMovio = false;
	energia = 100;
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
//recibe danio, disminuye la energia
void Player::hacerDanio(char value) {
	this->energia = this->energia - value;
}
//devuelve la energia
char Player::getEnergia() {
	return this->energia;
}
bool Player::estaVivo() {
	if (energia> 0)
		return true;
	return false;
}

void Player::addTileRecorrido(short x, short y) {
	// ToDo: Esto se puede mejorar usando sort+binary search
	auto& p = std::make_pair(x, y);
	if(std::find(tiles_recorridos.begin(), tiles_recorridos.end(), p) == tiles_recorridos.end()) {
		this->tiles_recorridos.push_back(p);
	}
}

bool Player::existsTileRecorrido(short x, short y) {
	// ToDo: Esto se puede mejorar usando sort+binary search
	auto& p = std::make_pair(x, y);
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

void Player::reiniciar() {
	x = xSiguiente = xInicial;
	y = ySiguiente = yInicial;
	bolaDeCristal = golem = false;
	tiles_recorridos.clear();
	velocidad = 0.18;
	radio = 125;
	energia = 100;
	magia = 100;
	congelado = false;
	terremoto =0;
	hielo= 0;
	bombas = 0;
	banderasAtrapadas.clear();
}

//////////////
PlayerManager::PlayerManager(){
	golem_count = 0;
}

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
		if(mapa.getTile(rand_x, rand_y)->isCaminable() && !mapa.tile_esta_ocupado(rand_x, rand_y, *this)) {
			found = true;
		}
		x++;
	}
	if(!found) {
		throw std::runtime_error("No se pudo ubicar el personaje\n");
	}else{
		p.setOnline();
		p.setPos(rand_x, rand_y);
		p.setPosInicial(rand_x, rand_y);
		//p.setPos(10,10);

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
		if(mapa.getTile(rand_x, rand_y)->isCaminable() && !mapa.tile_esta_ocupado(rand_x,rand_y,*this)) {
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
//metodos de Gole,
// Devuelve si un player existe
bool PlayerManager::golemExists(const std::string& nick) const {
	return golem_map.find(nick) != golem_map.end();
}

// Agrega un golem
void PlayerManager::addGolem(const std::string& nick, const std::string& tipo_pje, MapaServidor& mapa,int estrategiaElegida,string& nickDuenio,int posXduenio, int posYDuenio) {
	// Chequeo de sanidad
	assert(!golemExists(nick));
	Golem*& p = golem_map[nick];
	p = new Golem(nick, tipo_pje,estrategiaElegida,nickDuenio); 
	// Buscar posicion en el mapa
	bool found = false;
	int x = 0; // Contador de iteraciones
	int w, h; // Para guardar el tamaño del mapa
	mapa.getSize(&w, &h);
	//std::cout << "Mapa size: " << w << "," << h << "\n";
	// Limitamos la cantidad de iteraciones
	TileServidor* unTile;
	int cantIteraciones = 10;
	for (int i = 0 ; i< cantIteraciones;i++){
		//busco un tile cercano libre
		if(mapa.tileExists(posXduenio + i ,posYDuenio)){
			//derecha
			unTile = mapa.getTile(posXduenio + i ,posYDuenio);
			if (unTile->isCaminable() && !mapa.tile_esta_ocupado(posXduenio + i ,posYDuenio,*this)){
				found = true;
				break;
			}
		}
		if(mapa.tileExists(posXduenio - i ,posYDuenio)){
			//izquierda
			unTile = mapa.getTile(posXduenio - i ,posYDuenio);
			if (unTile->isCaminable() && !mapa.tile_esta_ocupado(posXduenio - i ,posYDuenio,*this)){
				found = true;
				break;
			}
		}
		if(mapa.tileExists(posXduenio ,posYDuenio + i)){
			//abajo
			unTile = mapa.getTile(posXduenio,posYDuenio + i );
			if (unTile->isCaminable() && !mapa.tile_esta_ocupado(posXduenio,posYDuenio + i ,*this)){
				found = true;
				break;
			}
		}
		if(mapa.tileExists(posXduenio ,posYDuenio - i)){
			//arriba
			unTile = mapa.getTile(posXduenio,posYDuenio - i );
			if (unTile->isCaminable() && !mapa.tile_esta_ocupado(posXduenio,posYDuenio - i ,*this)){
				found = true;
				break;
			}
		}

	}

	
	if(!found) {
		throw std::runtime_error("No se pudo ubicar el personaje\n");
	}else{
		p->setOnline();
		p->setPos(unTile->get_x(), unTile->get_y());
		//cargo pos
		p->setPosSiguiente(unTile->get_x(), unTile->get_y());
	}
}

// Devuelva un jugador
Golem* PlayerManager::getGolem(const std::string& nick) {
	assert(golemExists(nick));
	return golem_map[nick];
}

GolemMapT& PlayerManager:: getGolems() {
	return this->golem_map;
}
