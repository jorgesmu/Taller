#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "config_pantalla.h"
#include "config_entidad.h"
#include "config_general.h"
using namespace std;

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
				cout << "LOG ERROR: atributo de la configuracion erroeneo: " << clave <<endl;
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
				config.set_ancho(valor);
			}else if (clave == "alto"){
				config.set_alto(valor);
			}else {
				cout << "LOG ERROR: atributo de pantalla erroeneo: " << clave <<endl;
			}
		}
   }

}
void operator >> (const YAML::Node& node, vector <config_entidad>& entidades) {
   //aca itero cada entidad	
    for(unsigned i=0;i<node.size();i++) {
	    config_entidad nuevaEntidad("","",-1,-1,-1,-1,-1,-1);
		//aca itero dentro de la entidad
		for(YAML::Iterator it=node[i].begin();it!=node[i].end();++it) {
			//leo los atributos de pantalla
			string clave;
			string valor;
			it.first() >> clave;
			it.second() >> valor;
			//y los asigno
			if (clave == "nombre"){
				nuevaEntidad.set_nombre(valor); 
			}else if (clave == "imagen"){
				nuevaEntidad.set_path_imagen(valor);
			}else if (clave == "ancho_base"){
				nuevaEntidad.set_ancho_base(atoi(valor.c_str()));
			}else if (clave == "alto_base"){
				nuevaEntidad.set_alto_base(atoi(valor.c_str()));
			}else if (clave == "pixel_ref_x"){
				nuevaEntidad.set_pixel_ref_x(atoi(valor.c_str()));
			}else if (clave == "pixel_ref_y"){
				nuevaEntidad.set_pixel_ref_y(atoi(valor.c_str()));
			}else if (clave == "fps"){
				nuevaEntidad.set_fps(atoi(valor.c_str()));
			}else if (clave == "delay"){
				nuevaEntidad.set_delay(atoi(valor.c_str()));
			}else {
				cout << "LOG ERROR: atributo de entidad erroeneo: " << clave <<endl;
			}
		}
		entidades.push_back(nuevaEntidad);
   }

}
void parser_nivel(char* path){
	//imprimo el documento para mirar si lo que imprime yaml esta bien
	imprimir_documento(path);


	cout << endl << "Test parsear completo." << endl << "Contenido parseado con Yaml: " << endl;
	//abro el documento y parseo el nodo
	std::ifstream fin(path);
	YAML::Parser parser(fin);
	YAML::Node doc;
	parser.GetNextDocument(doc);
	config_pantalla pantalla(-1,-1);
	vector <config_entidad> entidades;
	config_general config(-1,-1);
	if(const YAML::Node *pName = doc.FindValue("pantalla")) {
		cout << "pantalla existe" << endl;
		doc["pantalla"] >> pantalla;
		cout << endl <<"valores:ancho_" << pantalla.get_ancho() << ",alto_" << pantalla.get_alto() << endl; 
	}else{
		cout << "LOG ERROR: pantalla no existe" << endl;
	}

	if(const YAML::Node *pName = doc.FindValue("escenarios")) {
		cout << "escenarios existe" << endl;
	}else{
		cout << "LOG ERROR: escenarios no existe" << endl;
	}
	if(const YAML::Node *pName = doc.FindValue("entidades")) {
		doc["entidades"] >> entidades;
		cout << "entidades existe" << endl;
	}else{
		cout << "LOG ERROR: entidades no existe" << endl;
	}
	if(const YAML::Node *pName = doc.FindValue("configuracion")) {
		doc["configuracion"] >> config;
		cout << "configuracion existe" << endl;
	}else{
		cout << "LOG ERROR: configuracion no existe" << endl;
	}
}

void parsear(char* path){
	//ejemplos basado en: https://code.google.com/p/yaml-cpp/wiki/HowToParseADocument
	
	cout << endl << endl << "Comienza Yaml parsing de nivel." << endl;
	try{
		parser_nivel(path);
	}catch (exception e){
		cout << endl << endl << "Ocurrio un error con la Yaml";
	}

	return;
}