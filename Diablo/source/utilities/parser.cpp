#include "parser.h"
#include <iostream>
#include <fstream>

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
int parser_test() {
	//ejemplos tomados de https://code.google.com/p/yaml-cpp/wiki/HowToParseADocument
	try{
		cout << "Comienza Yaml parsing." << endl;
		char* path="../resources/levels/test.yaml";
		imprimir_documento(path);
		
		std::ifstream archivo(path);
		YAML::Parser parser(archivo);
		YAML::Node doc;

		parser.GetNextDocument(doc);
		std::string unValor;
		doc >> unValor;
		std::cout << endl << "El valor del escalar tomado con Yaml: " << unValor << std::endl;

	}catch (exception e){
		cout << "Ocurrio un error al utilizar la Yaml";
	}
		return 0;
}