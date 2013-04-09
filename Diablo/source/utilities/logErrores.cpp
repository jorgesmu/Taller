#include "logErrores.h"
#include <fstream>
#include <string>
logErrores::logErrores(string logPath){
	//creo el archivo de log con el path que se pasa por parametro
	conexion.open(logPath);
}
