#pragma once

#include <fstream>
#include "../yaml-cpp/include/yaml-cpp/yaml.h"
#include "config_servidor.h"
#include "../source/utilities/config_juego.h"
//Parsea un nivel
config_juego parsear_juego(char* path);
//Parsea el puerto del servidor
config_servidor parsear_red_server(char* path);
//Imprime un documento en consola, se utiliza para verificar si estan bien los datos parseados
// Usa la version de parser.h
//void imprimir_documento(char* path);