#pragma once

#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "mapaservidor.h"
#include "../source/utilities/vec2.h"

typedef std::vector< std::pair<short,short> > TilesRecorridos;

class Player {
	private:
	std::string nick;
	std::string tipo_personaje;
	bool isOnline;
	int x, y; // Pos en tiles logicos en el mapa
	TilesRecorridos tiles_recorridos;
	
	public:
	Player();
	~Player();
	void init(const std::string& nick, const std::string& pje);
	void setPos(int x, int y);
	int getX() const;
	int getY() const;
	std::string getTipo() const;
	bool isOn() const;
	void setOffline();
	void setOnline();

	void addTileRecorrido(short x, short y);
	TilesRecorridos& getTilesRecorridos();

};

class PlayerManager {
	private:
	// Hash con los jugadores que entraron al servidor
	std::map<std::string, Player> player_map;

	public:
	// Devuelve si un player existe
	bool playerExists(const std::string& nick) const;
	// Agrega un jugador
	// Inicializa en una posicion aleatoria del mapa
	void addPlayer(const std::string& nick, const std::string& tipo_pj, MapaServidor& mapa);
	// Devuelve un jugador
	Player& getPlayer(const std::string& nick);

};