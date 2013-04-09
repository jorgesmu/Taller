#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "config_pantalla.h"
#include "config_entidad.h"
#include "config_general.h"
#include "config_escenario.h"
#include "config_juego.h"
#include "logErrores.h"
using namespace std;
//creo log de errores
logErrores logErrores("log.txt");

//lee e imprime un archivo
void imprimir_documento(char* path){
		//se puede mejorar pero es solo una clase de test
		cout << endl << "Contenido del archivo:" << path << endl;
		char cadena[100];
		std::ifstream archivo(path);
		if (archivo.good()){
		
			while (!archivo.eof()){
				archivo >> cadena;
				cout<< cadena <<endl;
			}

		}
		archivo.close();
		return;
}
void operator >> (const YAML::Node& node, config_general& config) {

   for(unsigned i=0;i<node.size();i++) {
      	for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos de la configuracion
			string clave;
			int valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "vel_personaje"){
				config.set_vel_personaje(valor);
			}else if (clave == "margen_scroll"){
				config.set_margen_scroll(valor);
			}else {
				logErrores.escribir("atributo de configuracion erroeneo",clave,valor);
			}
		}
   }

}
void operator >> (const YAML::Node& node, config_pantalla& config) {

   for(unsigned i=0;i<node.size();i++) {
      	for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos de pantalla
			string clave;
			int valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "ancho"){
				//actualizo atributos
				config.completoAtributo();
				config.completo_Ancho();
				config.set_ancho(valor);
			}else if (clave == "alto"){
				//actualizo atributos
				config.completoAtributo();
				config.completo_Alto();
				config.set_alto(valor);
			}else {
				logErrores.escribir("atributo de pantalla erroeneo",clave , valor);
			}
		}
   }

}
void operator >> (const YAML::Node& node, vector <config_entidad>& entidades) {
   //aca itero cada entidad	
    for(unsigned i=0;i<node.size();i++) {
	    config_entidad nuevaEntidad("","",-1,-1,-1,-1,-1,-1);
		bool creoEntidad = false; //verifica si se ingreso algun parametro valido
		//aca itero dentro de la entidad
		for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos de entidad
			string clave;
			string valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "nombre"){
				creoEntidad = true;
				nuevaEntidad.set_nombre(valor); 
			}else if (clave == "imagen"){
				creoEntidad = true;
				nuevaEntidad.set_path_imagen(valor);
			}else if (clave == "ancho_base"){
				creoEntidad = true;
				nuevaEntidad.set_ancho_base(atoi(valor.c_str()));
			}else if (clave == "alto_base"){
				creoEntidad = true;
				nuevaEntidad.set_alto_base(atoi(valor.c_str()));
			}else if (clave == "pixel_ref_x"){
				creoEntidad = true;
				nuevaEntidad.set_pixel_ref_x(atoi(valor.c_str()));
			}else if (clave == "pixel_ref_y"){
				creoEntidad = true;
				nuevaEntidad.set_pixel_ref_y(atoi(valor.c_str()));
			}else if (clave == "fps"){
				creoEntidad = true;
				nuevaEntidad.set_fps(atoi(valor.c_str()));
			}else if (clave == "delay"){
				creoEntidad = true;
				nuevaEntidad.set_delay(atoi(valor.c_str()));
			}else {
				logErrores.escribir("atributo de entidad erroeneo",clave,valor);
			}
		}
		if (creoEntidad){
			//se ingreso por lo menos un atributo correcto
			entidades.push_back(nuevaEntidad);
		} else {
			//se ingreso una entidad con todos los atributos erroneos
				logErrores.escribir("Se ingreso una entidad que no contenia ningun atributo valido. Se elimino la misma" );
		}
	}

}
void parsear_protagonistas(const YAML::Node& node, config_escenario& unEscenario){
	//aca itero por cada protagonista
    for(unsigned i=0;i<node.size();i++) {
		config_entidad_en_juego unProtagonista("",-1,-1);
		bool creoEntidad = false; //verifica si se ingreso algun parametro valido
		//aca itero dentro de un protagonista
		for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos del protagonista
			string clave;
			string valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "tipoEntidad"){
				creoEntidad = true;
				unProtagonista.set_nombre(valor); 
			}else if (clave == "x"){
				creoEntidad = true;
				unProtagonista.set_pos_x( atoi(valor.c_str()) );
			}else if (clave == "y"){
				creoEntidad = true;
				unProtagonista.set_pos_y( atoi(valor.c_str()) );
			}else {
				logErrores.escribir( "atributo de protagonista erroeneo",clave,valor );
			}
		}

		if (creoEntidad){
			//se ingreso por lo menos un atributo correcto
			//agrego un protagonista al escenario
			unEscenario.agregar_protagonista(unProtagonista);
		} else {
			//se ingreso un protagonista con todos los atributos erroneos
			logErrores.escribir("Se ingreso un protagonista en el escenario que no contenia ningun atributo valido. Se elimino la misma" );
		}
   }
}
void parsear_entidades(const YAML::Node& node, config_escenario& unEscenario){
	//aca itero por cada entidad
    for(unsigned i=0;i<node.size();i++) {
		config_entidad_en_juego unaEntidad("",-1,-1);
		bool creoEntidad = false; //verifica si se ingreso algun parametro valido
		//aca itero dentro de una entidad
		for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos del protagonista
			string clave;
			string valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "entidad"){
				creoEntidad = true;
				unaEntidad.set_nombre(valor); 
			}else if (clave == "x"){
				creoEntidad = true;
				unaEntidad.set_pos_x( atoi(valor.c_str()) );
			}else if (clave == "y"){
				creoEntidad = true;
				unaEntidad.set_pos_y( atoi(valor.c_str()) );
			}else {
				logErrores.escribir( "atributo de entidad en escenario erroeneo",clave,valor );
			}
		}
		if (creoEntidad){
			//se ingreso por lo menos un atributo correcto
			//agrego una entidad al escenario
			unEscenario.agregar_entidad(unaEntidad);
		} else {
			//se ingreso un protagonista con todos los atributos erroneos
			logErrores.escribir("Se ingreso una entidad en el escenario que no contenia ningun atributo valido. Se elimino la misma" );
		}
   }
}
void operator >> (const YAML::Node& node, vector <config_escenario>& escenarios) {
    //aca itero para cada escenario	
    for(unsigned i=0;i<node.size();i++) {
		//aca itero dentro del escenario
		config_escenario unEscenario("",-1,-1);
		for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			string clave;
			string valor;
			it.first() >> clave;

			//guardo los atributos
			if ( clave == "nombre" ){

				unEscenario.set_nombre(valor);
			} else if (clave == "size_x"){
				it.second() >> valor;
				unEscenario.set_tam_x(atoi(valor.c_str()));
			} else if (clave == "size_y"){
				it.second() >> valor;
				unEscenario.set_tam_y(atoi(valor.c_str()));
			} else if (clave == "protagonista"){
				parsear_protagonistas(it.second(),unEscenario);
			}else if (clave == "entidadesDef"){
				parsear_entidades(it.second(),unEscenario);
			}else{ 
				it.second() >> valor;
				logErrores.escribir( "atributo de escenario erroeneo",clave,valor ); 
			}
		}
		//agrego un escenario
		escenarios.push_back(unEscenario);
    }
}
//verifica que no se haya enviado algun nodo ppal incorrecto
void verificar_tags_ppales(YAML::Node& nodoPpal){
	for(YAML::Iterator it=nodoPpal.begin();it!=nodoPpal.end();++it) {
		std::string clave, valor;
		it.first() >> clave;
		if ( (clave != "pantalla") && (clave != "configuracion") &&  (clave != "escenarios") && (clave != "entidades") ){
			logErrores.escribir( "Se ingreso nodo principal erroneo ", clave );
		}
	}
	return;
}
config_juego parser_nivel(char* path){

	//imprimo el documento para mirar si lo que imprime yaml esta bien
	imprimir_documento(path);


	cout << endl << "Test parsear completo." << endl << "Contenido parseado con Yaml: " << endl;
	//abro el documento y parseo el nodo
	std::ifstream archivo(path);
	YAML::Parser parser(archivo);
	YAML::Node doc;
	parser.GetNextDocument(doc);
	//declaro variables resultado
	config_pantalla pantalla(-1,-1);
	vector <config_entidad> entidades;
	config_general config(-1,-1);
	vector <config_escenario> escenarios;
	config_juego juego;


	//parseo pantalla
	if(const YAML::Node *pName = doc.FindValue("pantalla")) {
		cout << "pantalla existe" << endl;
		doc["pantalla"] >> pantalla;
		juego.set_pantalla(pantalla);
		cout << endl <<"valores:ancho_" << pantalla.get_ancho() << ",alto_" << pantalla.get_alto() << endl; 
	}else{
		logErrores.escribir("No existe el valor pantalla");
	}
	//parseo escenarios
	if(const YAML::Node *pName = doc.FindValue("escenarios")) {
		cout << "escenarios existe" << endl;
		doc["escenarios"] >> escenarios;
		juego.set_escenarios(escenarios);
	}else{
		logErrores.escribir("No existe el valor escenarios");
	}
	//parseo entidades
	if(const YAML::Node *pName = doc.FindValue("entidades")) {
		doc["entidades"] >> entidades;
		juego.set_entidades(entidades);
		cout << "entidades existe" << endl;
	}else{
		logErrores.escribir( "No existe el valor entidades." );
	}
	//parseo configuracion
	if(const YAML::Node *pName = doc.FindValue("configuracion")) {
		doc["configuracion"] >> config;
		juego.set_configuracion(config);
		cout << "configuracion existe" << endl;
	}else{
		logErrores.escribir("No existe el valor configuracion" );
	}
	
	//verificar errores 
	verificar_tags_ppales(doc);
	logErrores.verificar_errores(pantalla,logErrores);
	//cierro log
	logErrores.cerrarConexion();
	//cerramos la conexion
	archivo.close();

	return juego;
}

config_juego parsear(char* path){
	//ejemplos basado en: https://code.google.com/p/yaml-cpp/wiki/HowToParseADocument
	
	cout << endl << endl << "Comienza Yaml parsing de nivel." << endl;
	config_juego juego;
	try{
		juego = parser_nivel(path);
	}catch (exception e){
		logErrores.escribir( "Ocurrio un error con la Yaml. El formato no corresponde a un archivo YAML" );
		//cierro log
		logErrores.cerrarConexion();
	}

	return juego;
}