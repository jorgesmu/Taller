#pragma once
#include <fstream>
#include <string>
using namespace std;
class logErrores {
	private:
		ofstream conexion;
	public:
		//Constructor
		logErrores(string logPath);
		//escribe el error en el log
		void escribir(string linea){
			conexion << "-ERROR: " << linea << "." << endl;
		}
		//escribe el error seguido del parametro en el log
		void escribir(string linea, string parametro){
			conexion << "-ERROR: "<< linea << ": " << parametro << "." << endl;
		}
		//escribe el error seguido del parametro en el log y su valor para parametros de tipo string
		void escribir(string linea, string parametro, string valor){
			conexion << "-ERROR: "<< linea << ": " << parametro << " con valor: " << valor << "." << endl;
		}
		//escribe el error seguido del parametro en el log y su valor para parametros de tipo int
		void escribir(string linea, string parametro, int valor){
			conexion << "-ERROR: "<< linea << ": " << parametro << " con valor: " << valor << "." << endl;
		}
		//cierra la conexion
		void cerrarConexion(){
			conexion.close();
		}

};