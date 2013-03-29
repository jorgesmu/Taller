#include "parser.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
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
void parser_test_lista() {


	char* path="../resources/levels/test2.yml";
	imprimir_documento(path);

	cout << endl << "Contenido parseado con Yaml:" << endl;
	
	std::ifstream archivo(path);
	YAML::Parser parser(archivo);
	YAML::Node doc;    // already parsed
	bool test=parser.GetNextDocument(doc);

	for(YAML::Iterator it=doc.begin();it!=doc.end();it ++) {
		std::string scalar;
		*it >> scalar;
		std::cout << "Found scalar: " << scalar << std::endl;
	}
	archivo.close();

}
void parser_test_escalar() {

		char* path="../resources/levels/test.yaml";
		imprimir_documento(path);

		std::ifstream archivo(path);
		YAML::Parser parser(archivo);
		YAML::Node doc;

		parser.GetNextDocument(doc);
		std::string unValor;
		doc >> unValor;
		std::cout << endl << "El valor del escalar tomado con Yaml: " << unValor << std::endl;

		archivo.close();



	return;
}
void parser_test(){
	//ejemplos tomados de https://code.google.com/p/yaml-cpp/wiki/HowToParseADocument

	cout << "Comienza Yaml parsing." << endl;
	try{
		parser_test_escalar();
		parser_test_lista();
	}catch (exception e){
		cout << endl << endl << "Ocurrio un error con la Yaml";
	}
}