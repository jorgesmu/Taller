#include "logErroresServer.h"

logErrores::logErrores(string logPath){
	//creo el archivo de log con el path que se pasa por parametro
	conexion.open(logPath);
}
void logErrores::escribir(string linea){
	conexion << "-ERROR: " << linea << "." << endl;
}