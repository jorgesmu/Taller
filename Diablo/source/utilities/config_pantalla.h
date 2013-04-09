#pragma once

#include <string>

class config_pantalla {

	private:
		int ancho,alto;
		bool completoAncho, completoAlto;
	public:
		//Constructores
		config_pantalla(int ancho_i, int alto_i);
		config_pantalla();
		//Getters
		int get_ancho() { return ancho;}

		int get_alto() { return alto;}

		bool get_completo_alto() { return completoAlto;}
		
		bool get_completo_ancho() { return completoAncho;}
		
		//setters
		void set_alto(int nuevaMedida){
			alto = nuevaMedida;
		}
		void set_ancho(int nuevaMedida){
			ancho = nuevaMedida;
		}
		//metodos para completar validaciones
		void completo_Alto(){
			completoAlto = true;
		}
		void completo_Ancho(){
			completoAncho = true;
		}
};