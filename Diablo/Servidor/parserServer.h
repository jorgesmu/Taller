#pragma once

#include <fstream>
#include "../yaml-cpp/include/yaml-cpp/yaml.h"
#include "config_servidor.h"

//Parsea el puerto del servidor
config_servidor parsear_red(char* path);
//Imprime un documento en consola, se utiliza para verificar si estan bien los datos parseados
void imprimir_documento(char* path);