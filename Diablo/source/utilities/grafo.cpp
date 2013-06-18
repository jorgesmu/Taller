#include "grafo.h"
#include "arista.h"
#include <queue>
#include <algorithm>
const int infinito = 999;
const int huerfano =-1; //huerfano = no tiene padre
using namespace std;
grafo::grafo(){};
grafo::grafo(int tamAncho,int tamAlto){
	ancho = tamAncho;
	alto = tamAlto;
	//matriz (columna=ancho,fila=alto)
	//resize llama al constructor
	vertices.resize(ancho);
	distancia.resize(ancho);
	visitado.resize(ancho);
	padre.resize(ancho);

	for (int indice=0;indice<alto;indice++){
		vector<vertice>& unaColumnaVertices = vertices.at(indice);
		vector<double>& unaColumnadistancia = distancia.at(indice);
		vector<bool>& unaColumnaVisitado= visitado.at(indice);
		vector<pair<int,int> >& unaColumnaPadre= padre.at(indice);
		
		unaColumnaVertices.resize(alto);
		unaColumnadistancia.resize(alto);
		unaColumnaVisitado.resize(alto);
		unaColumnaPadre.resize(alto);
	}
	//creo vertices
	for(int ind_ancho = 0; ind_ancho<ancho;ind_ancho++){
		for(int ind_alto = 0; ind_alto<ancho;ind_alto++){
			//cargo x e y de cada vertice
			vertice& unVertice = vertices[ind_ancho][ind_alto];
			unVertice.set_x(ind_ancho);
			unVertice.set_y(ind_alto);
		}
	}
}
//agrega una arista a un vertice de un grafo
void grafo::agregar_arista (int pos_x_vertice, int pos_y_vertice,int pos_x_arista, int pos_y_arista,double peso_arista){
	vertice& unVertice = vertices.at(pos_x_vertice).at(pos_y_vertice) ;
	unVertice.agregar_arista(pos_x_arista,pos_y_arista,peso_arista);
}
//getters
//devuelve los vectores
vector< vector<vertice> >& grafo::get_vertices(){
	return vertices;
}
#include <iostream>
//actualiza el valor de la arista, si dicha arista existe
void grafo::actualizar_arista (int pos_x_origen, int pos_y_origen,int pos_x_destino, int pos_y_destino,double peso_arista){
	vertice& unVertice = vertices[pos_x_origen][pos_y_origen];
	vector <arista>& aristas = unVertice.get_aristas();
	bool encontrada = false;
	int indice = 0;
	while (indice < aristas.size() && !encontrada){
		//busco arista
		arista& unaArista = aristas[indice];
		int xArista = unaArista.get_x_destino();
		int yArista = unaArista.get_y_destino();
		if(xArista == pos_x_destino && yArista == pos_y_destino){
			encontrada = true;
			unaArista.set_peso(peso_arista);
			//std::cout << "Seteando peso " << peso_arista << " a la arista de " << pos_x_origen << ";" << pos_y_origen << " -> " << pos_x_destino << ";" << pos_y_destino << "\n";
		}
		indice ++;
	}
}
	
//devuelve un vertice 
vertice& grafo::get_vertice(int pos_x,int pos_y){
	return vertices.at(pos_x).at(pos_y);
}

struct Par {
	vertice vertice;
	double distancia;
} ;
//otros
struct comparar  
{  
  bool operator()(const Par& primero, const Par& segundo)  
  {  
	  return primero.distancia > segundo.distancia;  
  }  
};  
//busca camino de atras para adelante
vector<pair<int,int> > grafo::reconstruir_camino(int posXOrigen,int posYOrigen, int posXDestino,int posYDestino){
	vector<pair<int,int> > resultado;
	int padreVerticeX = posXDestino;
	int padreVerticeY = posYDestino;
	bool error = false; //para caso borde que ocurre a veces de que no haya camino
	while (!((padreVerticeX == posXOrigen) && (padreVerticeY == posYOrigen))){
		pair<int,int> padreVertice;
		//validacion para caso borde de que no haya camino
		if(padreVerticeX >= 0 && padreVerticeY >= 0 ){
			padreVertice = padre[padreVerticeX][padreVerticeY];
			if(!((padreVerticeX == posXOrigen) && (padreVerticeY == posYOrigen)))
				resultado.push_back(padreVertice);
			padreVerticeX = padreVertice.first;
			padreVerticeY = padreVertice.second;
		}else{
			error = true;
		}
	}
	reverse(resultado.begin(),resultado.end());
	pair<int,int> destino = make_pair<int,int> (posXDestino,posYDestino);
	resultado.push_back(destino);
	if (error){
		vector<pair<int,int> > resVacio;
		return resVacio;
	}
	return resultado;
}

//devuelve el camino minimo entre un nodo inicial y uno final
vector<pair<int,int> > grafo::camino(int posXOrigen,int posYOrigen, int posXDestino,int posYDestino){
	for (int idx_ancho = 0; idx_ancho < ancho;idx_ancho++){
			for (int idx_alto = 0; idx_alto < ancho;idx_alto++){
				//inicializo padres, distancias, visitados
				distancia[idx_ancho][idx_alto] = infinito;
				padre[idx_ancho][idx_alto] = make_pair<int,int>(huerfano,huerfano);
				visitado [idx_ancho][idx_alto] = false;
			}
	}
	//encolo el origen
	distancia[posXOrigen][posYOrigen] = 0;
	priority_queue< Par,vector< Par >,comparar > cola;
	Par parInicial;
	parInicial.vertice = vertices[posXOrigen][posYOrigen];
	parInicial.distancia = 0;

	cola.push(parInicial); // push v[origen],dist[origen]
	bool termino = false;
	while(!cola.empty() && !termino){
		Par unPar = cola.top();
		vertice unVertice = unPar.vertice;
		cola.pop();
		int pos_x_vertice = unVertice.get_x();
		int pos_y_vertice = unVertice.get_y();

		if(pos_x_vertice == posXDestino && pos_y_vertice == posYDestino){
			termino = true;
		}

		vector<arista> aristas = unVertice.get_aristas();
		visitado[pos_x_vertice][pos_y_vertice] = true; // lo visito
		//recorro todas las aristas
		for(int idx_arista = 0;idx_arista < aristas.size();idx_arista++){
	
			int destinoAristaX = aristas[idx_arista].get_x_destino();
			int destinoAristaY = aristas[idx_arista].get_y_destino();
			if (!visitado[destinoAristaX][destinoAristaY]){
				double peso = aristas[idx_arista].get_peso();

				if (distancia[destinoAristaX][destinoAristaY] > distancia[pos_x_vertice][pos_y_vertice] + peso)
				{
					
					distancia[destinoAristaX][destinoAristaY] = distancia[pos_x_vertice][pos_y_vertice] + peso;
					padre[destinoAristaX][destinoAristaY] = make_pair<int,int>(pos_x_vertice,pos_y_vertice);
				
					Par nuevoPar;
					nuevoPar.vertice = vertices[destinoAristaX][destinoAristaY];
					nuevoPar.distancia = distancia[destinoAristaX][destinoAristaY];

					cola.push(nuevoPar); // push v[destino]
				}
			}
		}
	}
	//reconstruyo el camino
	vector<pair<int,int> > resultado = reconstruir_camino(posXOrigen,posYOrigen,posXDestino,posYDestino);
	return resultado;

}
