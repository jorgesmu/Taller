#pragma once
#include "playerman.h"
#include "mapaservidor.h"
#include "serversocket.h"
#include "../source/utilities/timer.h"
class Enemigo:public Player{
	protected:
		unsigned int estrategia,direccion;
		int pasosCaminados;
		Timer ultimoAtaque;
		bool ultimaAccionAtacar;
	public:
		unsigned int lineaRecta;
		void init_Enemy(const std::string& nickname,const std::string& tipo,unsigned int estrategiaElegida);
		//getters
		TileServidor* Enemigo::get_proximo_tile_recta_horizontal(MapaServidor& Mapa,PlayerManager& pm);
		TileServidor* Enemigo::get_proximo_tile_recta_vertical(MapaServidor& Mapa,PlayerManager& pm);
		TileServidor* Enemigo::get_proximo_tile_circular(MapaServidor& Mapa,PlayerManager& pm);
		virtual TileServidor* get_proximo_tile_enemigo(MapaServidor& Mapa,PlayerManager& pm);
		virtual bool personaje_en_radio(MapaServidor& mapa,PlayerManager& pm, TileServidor*& tilePersonajes);
		virtual bool personaje_adyacente(MapaServidor& mapa,PlayerManager& pm, TileServidor*& tilePersonaje,string& enemigoAtacado);
		virtual void atacar(string& NickAtacado,PlayerManager& pm,ServerSocket& socks);
		Timer& get_timer_ataque(){
			return this->ultimoAtaque;
		}
		void set_ultima_accion_atacar(bool nuevoValor){
			this->ultimaAccionAtacar = nuevoValor;
		}
		bool get_ultima_accion_atacar(){
			return this->ultimaAccionAtacar;
		}

		/*void cambiarEstrategia(bool bajaVel) {
			if (bajaVel) {
				double disminucion = 75;
				this->velocidad*=(disminucion/100);
			} else {
				double aumento = 25+100;
				this->velocidad*=(aumento/100);
			}
		}*/
};