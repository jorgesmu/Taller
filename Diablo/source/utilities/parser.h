#pragma once

#include <fstream>
#include "yaml-cpp/yaml.h"
#include "config_juego.h"
#include "config_cliente.h"
//Parsea un nivel
config_juego parsear_juego(char* path);
//Parsea la Ip del servidor y el puerto de comunicacion con el mismo
config_cliente parsear_red(char* path);
//Imprime un documento en consola, se utiliza para verificar si estan bien los datos parseados
void imprimir_documento(char* path);