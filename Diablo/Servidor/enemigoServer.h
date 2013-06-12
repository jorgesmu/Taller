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
		TileServidor* Enemigo::get_proximo_tile_recta_horizontal(MapaServidor& Mapa);
		TileServidor* Enemigo::get_proximo_tile_recta_vertical(MapaServidor& Mapa);
		TileServidor* Enemigo::get_proximo_tile_circular(MapaServidor& Mapa);
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

		void cambiarEstrategia() {
			if (estrategia == 1) {
				estrategia = 2;
			} else {
				estrategia = 1;
			}
		}
};