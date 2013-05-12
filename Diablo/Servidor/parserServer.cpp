#pragma once
#include "parserServer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "config_servidor.h"
#include "logErroresServer.h"
using namespace std;
const int errorConstante = -1;
//VARIABLES GLOBALES
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
void operator >> (const YAML::Node& node, config_servidor& config) {

   for(unsigned i=0;i<node.size();i++) {
      	for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos de pantalla
			string clave;
			string valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "puerto"){
				//actualizo atributos
				if( err_log.esNumerico(valor) ){
					config.set_puerto(atoi(valor.c_str()));
				}else{
					config.set_puerto(errorConstante);
				}

			}else {
				err_log.escribir("atributo de red erroeneo",clave , valor);
			}
		}
   }

}
//verifica que no se haya enviado algun nodo ppal incorrecto
void verificar_tags_ppales_red(YAML::Node& nodoPpal){
	for(YAML::Iterator it=nodoPpal.begin();it!=nodoPpal.end();++it) {
		std::string clave, valor;
		it.first() >> clave;
		if ( (clave != "red")){
			err_log.escribir( "Se ingreso nodo principal erroneo ", clave );
		}
	}
	return;
}

config_servidor parsear_red(char* path){
	//ejemplos basado en: https://code.google.com/p/yaml-cpp/wiki/HowToParseADocument
	//declaro variables resultado
	config_servidor configuracion(8080);

	try{
		//abro el documento y parseo el nodo
		std::ifstream archivo(path);
		YAML::Parser parser(archivo);
		YAML::Node doc;
		parser.GetNextDocument(doc);
	
		//parseo configuracion de red
		if(const YAML::Node *pName = doc.FindValue("red")) {
			doc["red"] >> configuracion;
		}else{
			err_log.escribir("No existe el valor red para conectarse con el server");
		}
		verificar_tags_ppales_red(doc);
		err_log.verificar_errores(configuracion,err_log);
	} catch(exception& ){
		//dejo en un estado valido si fallo
		err_log.verificar_errores(configuracion,err_log);
	}
	return configuracion;
}