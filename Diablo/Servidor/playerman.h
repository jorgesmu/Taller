#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <list>
#include <algorithm>

#include "mapaservidor.h"
#include "../source/utilities/vec2.h"
class Enemigo;
typedef std::vector< std::pair<short,short> > TilesRecorridos;

class Player {
	private:
	std::string nick;
	std::string tipo_personaje;
	bool isOnline;
	int x, y,xSiguiente,ySiguiente; // Pos en tiles logicos en el mapa
	bool bolaDeCristal, golem;
	bool seMovio;
	TilesRecorridos tiles_recorridos;
	double velocidad;
	float radio;
	char energia,magia,energiaEscudo;
	char congelado; //1=congelado,0=no congelado
	char terremoto,hielo; //cant de hechizos de cada tipo
	std::list<std::pair<int,int>> banderasAtrapadas; //para la mision de atrapar banderas
	std::string lastDamagedBy;

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
	int getXSiguiente() const ;
	int getYSiguiente() const ;
	void setPosSiguiente( int XAntiguo,int YAntiguo);
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
	bool getBolaDeCristal() { return bolaDeCristal; }
	void setBolaDeCristal(bool tieneBolaDeCristal) { bolaDeCristal=tieneBolaDeCristal; }
	bool tieneGolem() { return golem; }
	void setGolem(bool agarroGolem) { golem=agarroGolem; }
	bool getSeMovio() { return seMovio; }
	void setSeMovio(bool yaSeMovio) { seMovio = yaSeMovio; }
	//Agrega una nueva bandera atrapada por el jugador
	void atrapoBandera(int x, int y);
	//Se fija si ya atrapo la bandera dada
	bool tieneBandera(int x, int y);
	//Cantidad de banderas atrapadas
	int cantBanderas();
	//Para saber el ultimo que me ataco por si cumple mision
	void atacadoPor(std::string atacker) { lastDamagedBy=atacker; }
	std::string ultimoAtacante() { return lastDamagedBy; }
	//recibe danio, disminuye la energia
	void hacerDanio(char value);
	char getEnergia();
	bool estaVivo();
	void addTileRecorrido(short x, short y);
	bool Player::existsTileRecorrido(short x, short y);
	TilesRecorridos& getTilesRecorridos();

};

typedef std::map<std::string, Player> PlayerMapT;
typedef std::map<std::string, Enemigo*> EnemyMapT;

class PlayerManager {
	private:
	// Hash con los jugadores que entraron al servidor
	PlayerMapT player_map;
	map<std::string, Enemigo*> enemy_map;
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

	// Devuelve si un enemigo existe
	bool enemyExists(const std::string& nick) const;
	// Agrega un jugador
	// Inicializa en una posicion aleatoria del mapa
	void addEnemy(const std::string& nick, const std::string& tipo_pj, MapaServidor& mapa,int estrategiaElegida);
	// Devuelve un jugador
	Enemigo* getEnemy(const std::string& nick);
	// Devuelve el mapa de jugadores
	EnemyMapT& getEnemies();

};