#include "logErrores.h"

logErrores::logErrores(string logPath){
	//creo el archivo de log con el path que se pasa por parametro
	conexion.open(logPath);
}
