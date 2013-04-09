#pragma once
#include <fstream>
#include <string>
//incluyo estructuras a controlar
#include "config_pantalla.h"
#include "config_entidad.h"
#include "config_general.h"
#include "config_escenario.h"
using namespace std;
//se declaran los valores por defecto
const unsigned int anchoDef = 100;
const unsigned int altoDef = 100;
const unsigned int VelPerDef = 100; //velodidad personaje
const unsigned int scrollDef = 30;
const string imagenDef = "Path";
const unsigned int anchoElementoDef = 1;
const unsigned int altoElementoDef = 1;
const unsigned int pixelRefXDef= 1;
const unsigned int pixelRefYDef = 1;
const string nombreEntidadDef = "entidad desconocida";
const unsigned int fpsDef = 8;
const unsigned int delayDef = 10;
const string nombreEscenarioDef = "Lagos de Nepal";
const unsigned int sizeXDef = 100;
const unsigned int sizeYDef = 100;
const unsigned int posXDef = 10;
const unsigned int posYdef = 17;

class logErrores {
	private:
		ofstream conexion;
	public:
		//Constructor
		logErrores(string logPath);
		//escribe el error en el log
		void escribir(string linea){
			conexion << "-ERROR: " << linea << "." << endl;
		}
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
		//verfica errores de pantalla
		void verificar_errores(config_pantalla& pantalla, logErrores& logErrores){
			int alto = pantalla.get_alto();
			int ancho = pantalla.get_ancho();
			//verifico alto
			if ( (pantalla.get_completo_alto()) == false ){
				logErrores.escribir ("No se ingreso el alto de la pantalla, se selecciona valor por defecto");
				pantalla.set_alto(altoDef);
			}else if ( alto < 0 ){
				logErrores.escribir ("El alto de la pantalla ingresado es menor que la unidad, se selecciona valor por defecto") ;
				pantalla.set_alto(altoDef);			
			}
			//verifico ancho
			if ( (pantalla.get_completo_ancho()) == false ){
				logErrores.escribir ("No se ingreso el ancho de la pantalla, se selecciona valor por defecto");
				pantalla.set_alto(anchoDef);
			}else if ( ancho < 0 ){
				logErrores.escribir ("El ancho de la pantalla ingresado es menor que la unidad, se selecciona valor por defecto");
				pantalla.set_ancho(anchoDef);			
			}

		}
		//verfica errores de configuracion
		void verificar_errores(config_general& config, logErrores& logErrores){
			int velocidad = config.get_vel_personaje();
			int margen = config.get_margen_scroll();
			//verifico velocidad del personaje
			if ( (config.get_completo_velocidad()) == false ){
				logErrores.escribir ("No se ingreso la velocidad del personaje en la configuracion, se selecciona valor por defecto");
				config.set_vel_personaje(VelPerDef);
			}else if ( velocidad < 0 ){
				logErrores.escribir ("La velocidad del personaje ingresada es menor que la unidad, se selecciona valor por defecto");
				config.set_vel_personaje(VelPerDef);		
			}
			//verifico margen del scroll
			if ( (config.get_completo_margen()) == false ){
				logErrores.escribir ("No se ingreso el margen del scroll en la configuracion, se selecciona valor por defecto");
				config.set_margen_scroll(scrollDef);
			}else if ( velocidad < 0 ){
				logErrores.escribir ("El margen del scroll ingresado es menor que la unidad, se selecciona valor por defecto");
				config.set_margen_scroll(scrollDef);
		
			}

		}
};