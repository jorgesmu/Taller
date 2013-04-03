#pragma once

#include <fstream>
#include "yaml-cpp/yaml.h"


//Parsea un nivel
void parsear(char* path);
//Imprime un documento en consola, se utiliza para verificar si estan bien los datos parseados
void imprimir_documento(char* path);