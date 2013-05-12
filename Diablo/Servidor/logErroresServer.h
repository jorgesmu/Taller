#pragma once
#include <fstream>
#include <string>
#include "config_servidor.h"
//incluyo estructuras a controlar
using namespace std;
//se declaran los valores por defecto
const unsigned int puertoDef = 8080;

//hay que mover la implementacion del .h al .cpp, se hace de esta forma ya que el compilador tira error sino!!
class logErrores {
	private:
		ofstream conexion;
	public:
		//Constructor
		logErrores(string logPath);
		//escribe el error en el log
		void escribir(string linea);
		//escribe el error seguido del parametro en el log
		void escribir(string linea, string parametro){
			conexion << "-ERROR: "<< linea << ": " << parametro << "." << endl;
		}
		//escribe el error seguido del parametro en el log y su valor para parametros de tipo string
		void escribir(string linea, string parametro, string valor){
			conexion << "-ERROR: "<< linea << ": " << parametro << " con valor: " << valor << "." << endl;
		}
		//escribe el error seguido del parametro en el log y su valor para parametros de tipo int
		void escribir(string linea, string parametro, int valor){
			conexion << "-ERROR: "<< linea << ": " << parametro << " con valor: " << valor << "." << endl;
		}
		//cierra la conexion
		void cerrarConexion(){
			conexion.close();
		}
		//verfica errores de configuracion de red del cliente
		void verificar_errores(config_servidor& config_red, logErrores& logErrores){
			int puerto = config_red.get_puerto();
			//verifico puerto
			if ( (config_red.get_completo_puerto()) == false ){
				logErrores.escribir ("No se ingreso el puerto, se selecciona valor por defecto");
				config_red.set_puerto(puertoDef);
			}else if ( puerto < 0 ){
				logErrores.escribir ("El puerto ingresado es menor o igual que la unidad, se selecciona valor por defecto");
				config_red.set_puerto(puertoDef);		
			}
			
		}

		bool esNumerico(string valor){
			bool res = true;
			const char* a = valor.c_str();
			
			//verifico si es numerico si se puede convertir y si es negativo 
			if ( atoi(a)<0)
				return true;
			for (unsigned int i = 0 ; i<strlen(a);i++){
				char b= a[i];
				if(!isdigit(a[i])){
					res = false;
				}
			}
			return res;
		}
};

// Para declararlo como global (se define en main)
extern logErrores err_log;