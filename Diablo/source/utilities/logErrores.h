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
		void escribir(string linea){
			conexion << linea << endl;
		}
		void cerrarConexion(){
			conexion.close();
		}

};