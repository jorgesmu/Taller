#include <string>
#include "../utilities/surface.h"
#include "SDL.h"

// Forward declaration
class ResMan;
class Camara;

class Entidad2 {

	private:
	std::string name; // Nombre de la entidad
	std::string imagen; // Path a la imagen
	int u, v; // Offset dentro del tile
	Surface* surf; // Puntero a la surface de esta entidad (traida desde el resource manager on load)
	int w, h; // Ancho y alto de la base en la grilla

	public:
	// Ctor
	Entidad2();
	// Settea el nombre y carga la surface
	void init(const std::string& name, const ResMan& rm);
	// Cambia la posicion de la entidad, dentro del tile
	void offset(int x, int y);
	// Getters para la posicion
	int getX() const;
	int getY() const;
	// Actualiza las cosas internas, si hubiese
	void update();
	// Dibuja la entidad, recibe la camara y el par (x,y) del tile en el que esta
	void blit(SDL_Surface* dest, const Camara& cam, int x, int y);

};