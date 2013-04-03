#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "config_pantalla.h"
#include "config_entidad.h"
using namespace std;
//estrutura de un vector de 3 dimensiones para correr los ejemplos de yaml-cpp
struct Vector_3_dim {
    float x, y, z;
};
struct Power {
   std::string name;
   int damage;
};
struct Monster {
   string name;
   Vector_3_dim position;
   vector <Power> powers;
};
void operator >> (const YAML::Node& node, Vector_3_dim& v)
{
    node[0] >> v.x;
    node[1] >> v.y;
    node[2] >> v.z;
}
void operator >> (const YAML::Node& node, Power& power) {
   node["name"] >> power.name;
   node["damage"] >> power.damage;
}
void operator >> (const YAML::Node& node, Monster& monster) {
   node["name"] >> monster.name;
   node["position"] >> monster.position;
   const YAML::Node& powers = node["powers"];
   for(unsigned i=0;i<powers.size();i++) {
      Power power;
      powers[i] >> power;
      monster.powers.push_back(power);
   }
}
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
		cout << "configuracion existe" << endl;
	}else{
		cout << "LOG ERROR: configuracion no existe" << endl;
	}
}
void parser_test_completo(){
	char* path="../resources/levels/test5.yaml";
	imprimir_documento(path);


	cout << endl << "Test parsear completo." << endl << "Contenido parseado con Yaml: " << endl;

   std::ifstream fin(path);
   YAML::Parser parser(fin);
   YAML::Node doc;
   parser.GetNextDocument(doc);
   for(unsigned i=0;i<doc.size();i++) {
      Monster monster;
      doc[i] >> monster;
      cout << monster.name << "\n";
	  cout << "posicion:(" << monster.position.x << "," << monster.position.y << "," << monster.position.z << ")" << endl;
	  for(unsigned i=0;i<monster.powers.size();i++) {
		//se imprimen los poderes (no se cuantos son)
		cout << "power:" <<monster.powers[i].name << ", damage:" << monster.powers[i].damage << endl;
	  }
	}
}
void parser_test_vector(){
	char* path="../resources/levels/test4.yaml";
	imprimir_documento(path);


	cout << endl << "Test parsear vector." << endl << "Contenido parseado con Yaml: " << endl;

	std::ifstream archivo(path);
	YAML::Parser parser(archivo);
	YAML::Node doc;    
	parser.GetNextDocument(doc);

	Vector_3_dim unVector;
	doc >> unVector;
	
	std::cout << "El valor de las componentes del vector es: (" << unVector.x << ", " << unVector.y << ", " << unVector.z << ")";

	archivo.close();

	

	return;
}
void parser_test_arrays(){
	char* path = "../resources/levels/test3.yaml";
	imprimir_documento(path);

	cout << endl << "Test parsear Array." << endl << "Contenido parseado con Yaml:" << endl;
	
	std::ifstream archivo(path);
	YAML::Parser parser(archivo);
	YAML::Node doc;    
	parser.GetNextDocument(doc);

	for(YAML::Iterator it=doc.begin();it!=doc.end();++it) {
		std::string clave, valor;
		it.first() >> clave;
		it.second() >> valor;
		std::cout << "Clave: " << clave << ", valor: " << valor << std::endl;
	}
	archivo.close();
	return;

}
void parser_test_lista() {


	char* path="../resources/levels/test2.yaml";
	imprimir_documento(path);

	cout << endl << "Test parsear Lista." << endl << "Contenido parseado con Yaml:" << endl;
	
	std::ifstream archivo(path);
	YAML::Parser parser(archivo);
	YAML::Node doc;
	parser.GetNextDocument(doc);

	for(YAML::Iterator it=doc.begin();it!=doc.end();it ++) {
		std::string scalar;
		*it >> scalar;
		std::cout << "Found scalar: " << scalar << std::endl;
	}
	archivo.close();
	return;
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
	std::cout << endl << "Test parsear escalar." << endl << "El valor del escalar tomado con Yaml: " << unValor << endl;

	archivo.close();



	return;
}
void parser_test(){
	//ejemplos tomados y basados en: https://code.google.com/p/yaml-cpp/wiki/HowToParseADocument
	/* Aclaracion: el codigo escrito hasta ahora son adaptaciones de los ejemplos que se encuentran en la pagina.
	   se hacen los ejemplos completos y se agregan couts para poder verificar facilmente la salida, tambien
	   se traducen algunas variables para hacer mas facil su comprension y algunos pocos cambios en
	   funcion de las necesidades que tendremos para realizar el trabajo*/

	cout << "Comienza Yaml parsing." << endl;
	try{
		parser_test_escalar();
		parser_test_lista();
		parser_test_arrays();
		parser_test_vector();
		parser_test_completo();
	}catch (exception e){
		cout << endl << endl << "Ocurrio un error con la Yaml";
	}

	return;
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