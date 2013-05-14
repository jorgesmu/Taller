#include "grafo.h"
#include "arista.h"
#include <queue>
#include <algorithm>
const int infinito = 999;
const int huerfano =-1; //huerfano = no tiene padre
using namespace std;
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
vector< vector<vertice> > grafo::get_vertices(){
	return vertices;
}
	
//devuelve un vertice 
vertice grafo::get_vertice(int pos_x,int pos_y){
	return vertices.at(pos_x).at(pos_y);
}

//otros
struct comparar  
{  
  bool operator()(const pair<vertice,double>& primero, const pair<vertice,double>& segundo)  
  {  
	  return primero.second > segundo.second;  
  }  
};  
//busca camino de atras para adelante
vector<pair<int,int> > grafo::reconstruir_camino(int posXOrigen,int posYOrigen, int posXDestino,int posYDestino){
	vector<pair<int,int> > resultado;
	int padreVerticeX = posXDestino;
	int padreVerticeY = posYDestino;
	while (!((padreVerticeX == posXOrigen) && (padreVerticeY == posYOrigen))){
		pair<int,int> padreVertice = padre[padreVerticeX][padreVerticeY];
		if(!((padreVerticeX == posXOrigen) && (padreVerticeY == posYOrigen)))
			resultado.push_back(padreVertice);
		padreVerticeX = padreVertice.first;
		padreVerticeY = padreVertice.second;
	}
	reverse(resultado.begin(),resultado.end());
	pair<int,int> destino = make_pair<int,int> (posXDestino,posYDestino);
	resultado.push_back(destino);
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
	priority_queue< pair<vertice,double>,vector< pair<vertice,double> >,comparar > cola;
	pair<vertice, double> parInicial = make_pair<vertice,double>(vertices[posXOrigen][posYOrigen],0);
	cola.push(parInicial); // push v[origen],dist[origen]

	while(!cola.empty()){
		pair<vertice,double> unPar = cola.top();
		vertice unVertice = unPar.first;
		cola.pop();
		int pos_x_vertice = unVertice.get_x();
		int pos_y_vertice = unVertice.get_y();
		printf("\n\n\n desencolo %d,%d con peso:%f  \n\n\n",pos_x_vertice,pos_y_vertice, unPar.second);

		vector<arista> aristas = unVertice.get_aristas();
		visitado[pos_x_vertice][pos_y_vertice] = true; // lo visito
		//recorro todas las aristas
		for(int idx_arista = 0;idx_arista < aristas.size();idx_arista++){
	
			int destinoAristaX = aristas[idx_arista].get_x_destino();
			int destinoAristaY = aristas[idx_arista].get_y_destino();
			if (!visitado[destinoAristaX][destinoAristaY]){
				double peso = aristas[idx_arista].get_peso();

				distancia[destinoAristaX][destinoAristaY] = distancia[pos_x_vertice][pos_y_vertice] + peso;
				padre[destinoAristaX][destinoAristaY] = make_pair<int,int>(pos_x_vertice,pos_y_vertice);
				pair<vertice, double> nuevoPar = make_pair<vertice,double>(vertices[destinoAristaX][destinoAristaY],distancia[destinoAristaX][destinoAristaY]);
				cola.push(nuevoPar); // push v[destino]
				printf(" %d,%d es padre de %d,%d  \n\n",pos_x_vertice,pos_y_vertice,destinoAristaX,destinoAristaY);	
				printf("encolo %d,%d peso: %f  \n\n",destinoAristaX,destinoAristaY,peso);	
			}
		}
		system("PAUSE");
	}
	//reconstruyo el camino
	vector<pair<int,int> > resultado = reconstruir_camino(posXOrigen,posYOrigen,posXDestino,posYDestino);
	return resultado;

}
