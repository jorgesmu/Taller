#pragma once
#include <iostream>
#include <stdlib.h> 
#include <time.h> 
#include "../display/entidad.h"
#include "../utilities/ImagenPersonaje.h"
#include "../display/tile.h"
#include "../display/mapa.h"
#include "timer.h"
//#include "../utilities/corazon.h"
class Tile;
class Mapa;
class PjeManager;
class Corazon;
class Botella;
class Zapatos;
class Flechas;
//class Bombas;
class Granadas;
class Varitas;
class Escudo;
class BolaDeCristal;
class Lampara;
class MapaItem;
class Terremoto;
class Hielo;
class Bandera;
class ClientSocket;
class Arma;
class ArmaBomba;

class Personaje : public Entidad {
public:
	/*
		const static int RADIO_VISION_Y = 125;

		const static int RADIO_VISION_X = 2 * RADIO_VISION_Y; // En pixeles
	*/
		//Direccion
		const static unsigned int SUR = 0;
		const static unsigned int SURESTE = 1;
		const static unsigned int ESTE = 2;
		const static unsigned int NORESTE = 3; 
		const static unsigned int NORTE = 4;
		const static unsigned int NOROESTE = 5;
		const static unsigned int OESTE = 6;
		const static unsigned int SUROESTE = 7;	
		const static unsigned int CENTRO = 8;

		//Informacion de acciones
		const static unsigned int MOVER_EN_CURSO = 0;
		const static unsigned int MOVER_COMPLETADO = 1;
		const static unsigned int MOVER_ERROR = 2;
		//Atacar
		const static unsigned int ATACAR_EN_CURSO = 3;
		const static unsigned int ATACAR_COMPLETADO = 4;
		const static unsigned int ATACAR_ERROR = 5;
		//Defender
		const static unsigned int DEFENDER_EN_CURSO = 6;
		const static unsigned int DEFENDER_COMPLETADO = 7;
		const static unsigned int DEFENDER_ERROR = 8;
		//Freezar
		const static unsigned int FREEZAR_EN_CURSO = 9;
		const static unsigned int FREEZAR_COMPLETADO = 10;
		const static unsigned int FREEZAR_ERROR = 11;
		//Esperando accion
		const static unsigned int ESPERANDO_ACCION = 12;

		//Velocidad por defecto
		const static unsigned int VELOCIDAD_DEFAULT = 105; // En pixeles por segundo
		
		//Radio de los hechizos
		const static unsigned int RADIO_HECHIZO = 4; //en tiles

		//Revivir tiempo
		const static unsigned int TIEMPO_REVIVIR = 3000;

		//Daño maximo de los hechizos
		const static char DAÑO_TERREMOTO = 100;

		//Magia necesaria para un hechizo
		const static char MAGIA_HECHIZO = 50;

		//Vida inicial
		const static char ENERGIA_TOTAL = 100;

		//Magia inicial
		const static char MAGIA_TOTAL = 100;

		//Precision
		const static char PRECISION_PERSONAJE = 30;

	//	const static int DELTA_TIEMPO_UPDATE_POSICION = 20; // Tiempo entre cada update en clocks

		const static int MARGEN_ANCLA_X = 10;

		const static int MARGEN_ANCLA_Y = 10;

protected:

		Tile* tileDestino; // Tile destino

		double velocidad; // Pixeles por clock del procesador local

		clock_t deltaUpdatePosicion; // Distancia en ciclos del procesador local entre cada actualizacion

		clock_t tiempoProximoUpdate; // Tiempo proximo update en ciclos del procesador local

		bool actualizandoPosicion;

		bool cambioDireccionHabilitado;

		float radioY,radioX;

		bool PosicionActualizada; // para el update de enemigos

		//Tiene o no los hechizos(podria tener mas de uno quizas?, por eso un char)
		char terremoto;
		char hielo;

		//Vida restante
		char energia;

		//Magia
		char magia;

		//Para ataques
		char precision;

		//Armas
		int flechas;
		char bombas;
		int granadas;
		bool varita;
		char energiaEscudo;

		//Para la bomba
		Timer tBomba; //timer para que explote
		int posBombaX,posBombaY;
		ArmaBomba* bombaColocada;

		//Muerte
		bool vivo;

		//si tiene la bola de cristal
		bool bolaDeCristal;
		//si tiene golem
		bool golem;
		//Timer para revivir
		Timer timerRevivir;
		//timer para actualizar enemigos
		Timer timerUpdateServidor;
		// Arma default
		Arma* espada;
				
public:
	
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia con los atributos por defecto.
	*/
	Personaje();
		
	/*
		Pre:-
		 
		Post: Se ha inicializado la instancia segun el archivo y los par?etros.

	*/
	Personaje(const std::string& name, 
			const unsigned int wTiles , const unsigned int hTiles , 
			const unsigned int fps , const unsigned int delay , 
			const unsigned int altoSprite , const unsigned  int anchoSprite ,
			const unsigned int velocidad ,
			const int pixel_ref_x , const int pixel_ref_y,
			Tile* tile, 
			ResMan& rm , const int colorKey);

	/*
		Pre: La instancia ha sido creada.
	 
		Post: Se ha destruido la instancia liberando los recursos asociados.
	*/
	Personaje::~Personaje();

	/*
		Pre: Se ha creado la instancia.

		Post: Se ha seteado la instancia de acuerdo a los parametros.

		NOTA: ImagenAnimada
	*/
	virtual void init(const std::string& nickname, const std::string& name,
					const unsigned int wTiles , const unsigned int hTiles , 
					const unsigned int fps , const unsigned int delay ,
					const unsigned int altoSprite , const unsigned int anchoSprite ,
					const unsigned int velocidad ,
					const int pixel_ref_x , const int pixel_ref_y,
					Tile* tile, 
					ResMan& rm , const int colorKey);

	/*
		Pre: Los parámetros respetan la siguiente convención:

			"x" e "y": Coordenadas del Tile destino

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
	*/
	virtual Timer& get_timer_update(){
		return timerUpdateServidor; 
	}
	
	virtual void mover(Tile* tileDestino);

	/*
		Pre: Mapa distinto de null

		Post: Se ha encaminado el movimiento de la entidad al Tile correspondiente.

		Nota: Puede suceder que si una entidad ocupa varios Tiles la entidad se de de alta
		en algun Tile en el que no estaba, y se de de baja en alguno en cual estaba.
	*/
	virtual void mover(Tile* tileDestino , Mapa* mapa);
	
	/*
		Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
		dirección del ataque.

		Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

	*/
	virtual unsigned int ataque(Tile* tileDestino , Mapa* mapa);

	/*
		Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
		dirección del ataque.

		Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

	*/
	virtual unsigned int ataque(Tile* tileDestino , Mapa* mapa , Personaje* personajeObjetivo);

	/*
		Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
		dirección del ataque.

		Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

	*/
	virtual void freezar();

	// Valor = true freezea, valor = false; desfreezea
	virtual void freezar(bool valor);

	/*
		Pre: Mapa distinto de null. El parametro tileDestino es cualquier tile en la 
		dirección del ataque.

		Post: Se ha realizado un ataque en la direccion correspondiente del tile parametro.

	*/
	virtual unsigned int defender(Tile* tileDestino , Mapa* mapa);
	

	void animacionMuerte();

	void animacionRevivir();

	/*
		Pre: La instancia ha sido creada.
		Post: Se setea el Tile destino.
	*/
	virtual void setTileDestino(Tile* tile);

	virtual const std::string& getNick() const;

	//deprecated
	virtual void setTileActual(Tile* tile);
	
	virtual void setTileActual(Tile* tile , Mapa* mapa);

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna el tile donde se encuentra la instancia.
	*/
	virtual Tile* getPosicion(Mapa* mapa);

	/*
		Pre: Se ha creado la instancia.

		Post: Se actualiza el movimiento de la instancia.
	*/
	virtual unsigned int update(Mapa* mapa);

	//Con logica de timer revivir
	void updateRevivir();

	// Retorna el ancla de niebla X adecuada
	virtual int getXAnclajeNiebla();

	// Retorna el ancla de niebla Y adecuada
	virtual int getYAnclajeNiebla();
	virtual bool get_posicion_actualizada(){
		return PosicionActualizada;
	}
	virtual void set_posicion_actualizada(bool nuevoValor){
		PosicionActualizada = nuevoValor;
	}

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile donde se encuentra la
		instancia.
	*/
	virtual bool isCaminable(Tile* tile);

	/*
		Pre: La instancia ha sido creada.
		Post: Se retorna verdadero si se puede ocupar el tile donde se encuentra la
		instancia.
	*/
	// Deprecated
	virtual bool isCaminable();	

	virtual bool getBolaDeCristal();

	virtual void setBolaDeCristal(bool bolaDeCristal);

	virtual bool tieneGolem();

	virtual void setGolem(bool agarroGolem);

	/*
		Pre: Los parámetros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el que se trata de dibujar la entidad.
			NOTA: Cuidado al momento de hacer updates, ya que hay entidades que 
			ocupan varios Tiles. En sintesis, un update por entidad al momento
			de pintar toda la pantalla.

		Post: Se ha pintado la entidad en el surface dest según la camara y el mapa.

	*/
	virtual void blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY);

	/*
		Pre: Los parámetros cumplen las siguiente condiciones:

			dest: Surface sobre el que se quiere pintar.

			camara: Camara correspondiente.

			mapa: mapa correspondiente

			tileX , tileY : Tile sobre el que se trata de dibujar la entidad.
			NOTA: Cuidado al momento de hacer updates, ya que hay entidades que 
			ocupan varios Tiles. En sintesis, un update por entidad al momento
			de pintar toda la pantalla.

		Post: Se ha pintado la entidad en el surface dest según la camara y el mapa.

	*/
	virtual void blit(SDL_Surface* dest, Camara* camara , Mapa* mapa,
					const unsigned int tileX ,	const unsigned int tileY , bool color);
	
	//Colisiones
	
	void chocarConEntidad() { }

	void chocarConEntidadFija() { }

	void chocarConItem() { }

	void chocarConLampara(Lampara * lampara);

	void chocarConMapa(MapaItem* mapaItem);

	void aumentarVelocidad(char porcentaje);

	void chocarConZapatos(Zapatos* zapatos);

	void chocarConBolaDeCristal();

	void chocarConGolem();

	void chocarConHechizo() { }

	void chocarConTerremoto(Terremoto* terremoto);

	void chocarConHielo(Hielo* hielo);
	
	void chocarConCorazon(Corazon* corazon);

	void chocarConBotella(Botella* botella);

	void chocarConFlechas(Flechas* flechas);

//	void chocarConBombas(Bombas* bombas);

	void chocarConGranadas(Granadas* granadas);

	void chocarConVaritas(Varitas* varitas);

	void chocarConEscudo(Escudo* escudo);

	void chocarConBandera(Bandera* bandera);

	void setTerremoto(char valor) { this->terremoto=valor; }

	char getTerremoto() { return this->terremoto; }

	bool tieneTerremoto() { return this->terremoto>0; }
	
	void utilizarTerremoto(Mapa* mapa, PjeManager* pjm, ClientSocket* sock);

	void setHielo(char valor) { this->hielo=valor; }

	char getHielo() { return this->hielo; }

	bool tieneHielo() { return this->hielo>0; }

	void utilizarHielo(Mapa* mapa, PjeManager* pjm);

	bool estaVivo() {return vivo; }

	void revivir();

	void setPrecision(char new_precision) { precision=new_precision; }

	char getPrecision() { return precision; }

	//Setea el radio de vision en el eje Y
	void setRadio(float newRadio) { 
		radioY=newRadio;
		radioX=2*radioY;
	}

	//Recibe un valor de proporcion (0.25 aumenta 25%) para aumentar el radio de vision
	void aumentarRadio(float proporcion);

	float getRadioX() { return radioX; }

	float getRadioY() { return radioY; }

	char getEnergia() { return energia; }

	void setEnergia(char newEnergia) { energia=newEnergia; }

	char getMagia() { return magia; }

	void setMagia(char newMagia) { magia=newMagia; }

	char getEnergiaEscudo() { return energiaEscudo; }

	void setEnergiaEscudo(char energia) { energiaEscudo=energia; }

	double getVelocidad() { return velocidad; }
	
	void setVelocidad(double velocidad) { 
		this->velocidad=velocidad; 
	}

	// Para dañar un personaje
	void dañar(char daño); 

	Arma* getArmaActiva();

	void dejarItem(int x, int y, ResMan* resman);

	//Para la bomba
	char getCantBombas() { return bombas; }

	void setCantBombas(char cantidad) { bombas=cantidad; }

	void sumarBombas(char cantidad) { bombas+=cantidad; }

	void utilizarBomba(int xPersonaje, int yPersonaje);

	void updateBomba();

	void setBombaColocada(ArmaBomba* nuevaBomba) { bombaColocada=nuevaBomba; }

	ArmaBomba* getBombaColocada() { return bombaColocada; }

	void setBombaX(int pos) { posBombaX=pos; }
	
	int getBombaX() { return posBombaX; }

	void setBombaY(int pos) { posBombaY=pos; }

	int getBombaY() { return posBombaY; }
>>>>>>> 60890607cf02ff7703fd20261ab2bf2c9846aafd

protected:

	// Nombre del chabon
	std::string nickname;

	/*
		Pre: Los parametros definen el Tile destino.

		Post: Se retorna la dirección a la que se debe mover la entidad de acuerdo a la
		siguiente codificacion:

			SUR = 0;
			SURESTE = 1;
			ESTE = 2;
			NORESTE = 3; 
			NORTE = 4;
			NOROESTE = 5;
			OESTE = 6;
			SUROESTE = 7;	
			CENTRO = 8;
			
		NOTA: CENTRO implica que la entidad esta en el Tile destino y por lo tanto debe volver 
		aproximadamente al centro del mismo.
	*/
	virtual unsigned int calcularDireccion(Mapa* mapa);


	virtual unsigned int actualizarPosicion(Mapa* mapa);

	
	virtual void calcularPosicionTentativa(unsigned int direccion , 
									int* offsetTentativoX , int* offsetTentativoY);
		
	virtual void actualizarImagen(const unsigned int direccion);

	virtual Tile* obtenerTileAncla(const int posX , const int posY , 
								const unsigned int direccion , Mapa* mapa);

	/*
		Pre:-

		Post: Inicializa los atributos con los valores por defecto.

			name:  Cadena vacia.

			path: Cadena vacia.

			posTileX , posTileY: Ambos en cero.

			posPixelX , posPixelY: Ambos en cero.

			surf: Puntero nulo.

			imagen: Puntero nulo.

			widthInTiles , highInTiles: En 1.

			pixel_ref_x , pixel_ref_y: En los valores default dados por las constantes
										correspondientes.

	*/
	virtual void inicializarAtributosEnValoresDefault();
	
	/*
 		Pre: La instancia ha sido creada.

 		Post: Se retorna verdadero si se puede ocupar el tile destino de la instancia, caso contrario
		se retorna falso.
	*/
	bool verificarDestinoCaminable(Mapa* mapa);

	bool verificarAncla(Tile* ancla);

	//Sin tener en cuenta al escudo
	void dañarPersonaje(char daño);

	//Muerte del personaje
	void muere();

	

};