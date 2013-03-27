#include "parser.h"

//Intenta leer un archivo de prueba
int parser_test() {
	std::ifstream fin("test.yml");
    YAML::Parser parser(fin);

    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
       
    }
	
    return 0;
}