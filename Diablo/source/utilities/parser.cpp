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
#include "config_entidad_en_juego.h"
#include "logErrores.h"
using namespace std;

const int errorConstante = -1;

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
			string clave,valor;
		
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "vel_personaje"){
				config.completo_velocidad();
				if( err_log.esNumerico(valor) ){
					config.set_vel_personaje(atoi(valor.c_str()));
				}else{
					config.set_vel_personaje(errorConstante);
				}
			}else if (clave == "margen_scroll"){
				config.completo_Margen();
				if( err_log.esNumerico(valor) ){
					config.set_margen_scroll(atoi(valor.c_str()));
				}else{
					config.set_vel_personaje(errorConstante);
				}
			}else {
				err_log.escribir("atributo de configuracion erroeneo",clave,valor);
			}
		}
   }

}
void operator >> (const YAML::Node& node, config_pantalla& config) {

   for(unsigned i=0;i<node.size();i++) {
      	for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos de pantalla
			string clave;
			string valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "ancho"){
				//actualizo atributos
				config.completo_Ancho();
				if( err_log.esNumerico(valor) ){
					config.set_ancho(atoi(valor.c_str()));
				}else{
					config.set_ancho(errorConstante);
				}
			}else if (clave == "alto"){
				//actualizo atributos
				config.completo_Alto();
				if( err_log.esNumerico(valor) ){
					config.set_alto(atoi(valor.c_str()));
				}else{
					config.set_alto(errorConstante);
				}

			}else {
				err_log.escribir("atributo de pantalla erroeneo",clave , valor);
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
			//actualizo atributos
			//y los asigno
			if (clave == "nombre"){
				creoEntidad = true;
				nuevaEntidad.completo_nombre();
				nuevaEntidad.set_nombre(valor); 
			}else if (clave == "imagen"){
				creoEntidad = true;
				nuevaEntidad.completo_path();
				nuevaEntidad.set_path_imagen(valor);
			}else if (clave == "ancho_base"){
				creoEntidad = true;
				nuevaEntidad.completo_Ancho();
				if( err_log.esNumerico(valor) ){
					nuevaEntidad.set_ancho_base(atoi(valor.c_str()));
				}else{
					nuevaEntidad.set_ancho_base(errorConstante);
				}
			}else if (clave == "alto_base"){
				creoEntidad = true;
				nuevaEntidad.completo_Alto();
				if( err_log.esNumerico(valor) ){
					nuevaEntidad.set_alto_base(atoi(valor.c_str()));
				}else{
					nuevaEntidad.set_alto_base(errorConstante);
				}
			}else if (clave == "pixel_ref_x"){
				creoEntidad = true;
				if( err_log.esNumerico(valor) ){
					nuevaEntidad.set_pixel_ref_x(atoi(valor.c_str()));
				}else{
					nuevaEntidad.set_pixel_ref_x(errorConstante);
				}
				nuevaEntidad.set_pixel_ref_x(atoi(valor.c_str()));
			}else if (clave == "pixel_ref_y"){
				creoEntidad = true;
				nuevaEntidad.completo_pixelY();
				if( err_log.esNumerico(valor) ){
					nuevaEntidad.set_pixel_ref_y(atoi(valor.c_str()));
				}else{
					nuevaEntidad.set_pixel_ref_y(errorConstante);
				}
			}else if (clave == "fps"){
				creoEntidad = true;
				nuevaEntidad.completo_fps();
				if( err_log.esNumerico(valor) ){
					nuevaEntidad.set_fps(atoi(valor.c_str()));
				}else{
					nuevaEntidad.set_fps(errorConstante);
				}
				nuevaEntidad.set_fps(atoi(valor.c_str()));
			}else if (clave == "delay"){
				creoEntidad = true;
				nuevaEntidad.completo_delay();
				if( err_log.esNumerico(valor) ){
					nuevaEntidad.set_delay(atoi(valor.c_str()));;
				}else{
					nuevaEntidad.set_delay(errorConstante);
				}
			}else {
				err_log.escribir("atributo de entidad erroeneo",clave,valor);
			}
		}
		if (creoEntidad){
			//se ingreso por lo menos un atributo correcto
			entidades.push_back(nuevaEntidad);
		} else {
			//se ingreso una entidad con todos los atributos erroneos
				err_log.escribir("Se ingreso una entidad que no contenia ningun atributo valido. Se elimino la misma" );
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
				if( err_log.esNumerico(valor) ){
					unProtagonista.set_pos_x(atoi(valor.c_str()));
				}else{
					unProtagonista.set_pos_x(errorConstante);
				}
			}else if (clave == "y"){
				creoEntidad = true;
				if( err_log.esNumerico(valor) ){
					unProtagonista.set_pos_y(atoi(valor.c_str()));
				}else{
					unProtagonista.set_pos_y(errorConstante);
				}
			}else {
				err_log.escribir( "atributo de protagonista erroeneo",clave,valor );
			}
		}

		if (creoEntidad){
			//se ingreso por lo menos un atributo correcto
			//agrego un protagonista al escenario
			unEscenario.agregar_protagonista(unProtagonista);
		} else {
			//se ingreso un protagonista con todos los atributos erroneos
			err_log.escribir("Se ingreso un protagonista en el escenario que no contenia ningun atributo valido. Se elimino la misma" );
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
				if( err_log.esNumerico(valor) ){
					unaEntidad.set_pos_x( atoi(valor.c_str()) );
				}else{
					unaEntidad.set_pos_x(errorConstante);
				}
			}else if (clave == "y"){
				creoEntidad = true;
				if( err_log.esNumerico(valor) ){
					unaEntidad.set_pos_y( atoi(valor.c_str()) );
				}else{
					unaEntidad.set_pos_y(errorConstante);
				}
			}else {
				err_log.escribir( "atributo de entidad en escenario erroeneo",clave,valor );
			}
		}
		if (creoEntidad){
			//se ingreso por lo menos un atributo correcto
			//agrego una entidad al escenario
			unEscenario.agregar_entidad(unaEntidad);
		} else {
			//se ingreso un protagonista con todos los atributos erroneos
			err_log.escribir("Se ingreso una entidad en el escenario que no contenia ningun atributo valido. Se elimino la misma" );
		}
   }
}
void operator >> (const YAML::Node& node, vector <config_escenario>& escenarios) {
	bool control;
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
				if( err_log.esNumerico(valor) ){
					unEscenario.set_tam_x(atoi(valor.c_str()));
				}else{
					unEscenario.set_tam_x(errorConstante);
				}
			} else if (clave == "size_y"){
				it.second() >> valor;
				if( err_log.esNumerico(valor) ){
					unEscenario.set_tam_y(atoi(valor.c_str()));
				}else{
					unEscenario.set_tam_y(errorConstante);
				}
			} else if (clave == "protagonista"){
				parsear_protagonistas(it.second(),unEscenario);
			}else if (clave == "entidadesDef"){
				parsear_entidades(it.second(),unEscenario);
			}else{ 
				it.second() >> valor;
				err_log.escribir( "atributo de escenario erroeneo",clave,valor ); 
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
			err_log.escribir( "Se ingreso nodo principal erroneo ", clave );
		}
	}
	return;
}
config_juego parser_nivel(char* path){

	//imprimo el documento para mirar si lo que imprime yaml esta bien
	//imprimir_documento(path);


	//cout << endl << "Test parsear completo." << endl;
	cout << "Contenido parseado con Yaml: " << endl;
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
	}else{
		err_log.escribir("No existe el valor pantalla");
	}
	//parseo escenarios
	if(const YAML::Node *pName = doc.FindValue("escenarios")) {
		cout << "escenarios existe" << endl;
		doc["escenarios"] >> escenarios;
	}else{
		err_log.escribir("No existe el valor escenarios");
	}
	//parseo entidades
	if(const YAML::Node *pName = doc.FindValue("entidades")) {
		doc["entidades"] >> entidades;
		cout << "entidades existe" << endl;
	}else{
		err_log.escribir( "No existe el valor entidades." );
	}
	//parseo configuracion
	if(const YAML::Node *pName = doc.FindValue("configuracion")) {
		doc["configuracion"] >> config;
		cout << "configuracion existe" << endl;
	}else{
		err_log.escribir("No existe el valor configuracion" );
	}

	//verificar errores 
	verificar_tags_ppales(doc);
	err_log.verificar_errores(pantalla,err_log);
	err_log.verificar_errores(config,err_log);
	err_log.verificar_errores(entidades,err_log);
	err_log.verificar_errores(escenarios, err_log, entidades);
	err_log.verificar_unicidad_entidades (entidades, err_log);
	escenarios = err_log.verificar_correspondencia_escenario(err_log,entidades, escenarios);
	//asigno atributos al juego
 	juego.set_pantalla(pantalla);
	juego.set_escenarios(escenarios);
	juego.set_entidades(entidades);
	juego.set_configuracion(config);
	//cierro log
	err_log.cerrarConexion();
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
	}catch (exception& e){
		err_log.escribir("YAML exception catched. El archivo de configuracion no es YAML valido." );
		err_log.escribir(e.what());
		exit(-1);
		//cierro log
		err_log.cerrarConexion();
	}

	return juego;
}