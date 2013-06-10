/*#pragma	once


class Bombas: public Arma {
	
	private:
	
		char cantBombas;
	
	public:
	
		//Constructor
		Bombas();
		
		//Constructor
		Bombas(const std::string& name, 
			const unsigned int fps , const unsigned int delay ,
			const unsigned int velocidad ,
			const int pixel_ref_sprites_primario_x , const int pixel_ref_sprites_primario_y,
			Tile* tile, 
			ResMan& rm , const int colorKey , 
			char danio_maximo , Personaje* propietario):Arma(name, 
			fps , delay , velocidad , pixel_ref_sprites_primario_x , pixel_ref_sprites_primario_y,
			tile, rm , colorKey , danio_maximo , propietario);

		void chocarCon(Personaje* personaje) {
			personaje->chocarConBombas(this);
		}

		int getCantBombas() { return this->cantBombas; }
		
};
		char getCantBombas() { return this->cantBombas; }

};*/
