#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <list>
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
	double velocidad;
	float radio;
	char energia,magia,energiaEscudo;
	char congelado; //1=congelado,0=no congelado
	char terremoto,hielo; //cant de hechizos de cada tipo
	std::list<std::pair<int,int>> banderasAtrapadas; //para la mision de atrapar banderas

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
	std::string getNick() const;
	double getVelocidad() { return velocidad; }
	void setVelocidad(double newVel) { velocidad=newVel; }
	char getEnergia() { return energia; }
	void setEnergia(char newEnergia) { energia=newEnergia; }
	char getMagia() { return magia; }
	void setMagia(char newMagia) { magia=newMagia; }
	char getEnergiaEscudo() { return energiaEscudo; }
	void setEnergiaEscudo(char energia) { energiaEscudo=energia; }
	void congelar() { congelado=1; }
	void descongelar() {congelado=0; }
	bool isCongelado() { return congelado==1; }
	void sumarTerremoto() { terremoto++; }
	void restarTerremoto() { terremoto--; }
	char getTerremoto() { return terremoto; }
	void setTerremoto(char valor) { terremoto=valor; }
	void sumarHielo() { hielo++; }
	void restarHielo() { hielo--; }
	char getHielo() { return hielo; }
	void setHielo(char valor) { hielo=valor; }
	float getRadio() { return radio; }
	void setRadio(float newRadio) { radio=newRadio; }
	//Agrega una nueva bandera atrapada por el jugador
	void atrapoBandera(int x, int y);
	//Se fija si ya atrapo la bandera dada
	bool tieneBandera(int x, int y);
	//Cantidad de banderas atrapadas
	int cantBanderas();

	void addTileRecorrido(short x, short y);
	TilesRecorridos& getTilesRecorridos();

};

typedef std::map<std::string, Player> PlayerMapT;

class PlayerManager {
	private:
	// Hash con los jugadores que entraron al servidor
	PlayerMapT player_map;

	public:
	// Devuelve si un player existe
	bool playerExists(const std::string& nick) const;
	// Agrega un jugador
	// Inicializa en una posicion aleatoria del mapa
	void addPlayer(const std::string& nick, const std::string& tipo_pj, MapaServidor& mapa);
	// Devuelve un jugador
	Player& getPlayer(const std::string& nick);
	// Devuelve el mapa de jugadores
	PlayerMapT& getPlayers();

};