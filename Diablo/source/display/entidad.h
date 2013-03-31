#include <string>
#include "../utilities/surface.h"
#include "SDL.h"

class Entidad {

	private:
	std::string name; // Nombre de la entidad
	std::string imagen; // Path a la imagen
	int x, y; // Posicion en la grilla
	Surface surf; // Puntero a la surface de esta entidad (traida desde el resource manager on load)
	int w, h; // Ancho y alto de la base en la grilla

	public:
	// Ctor
	Entidad();
	// Settea el nombre y carga la surface
	void init(const std::string& name, const std::string& img);
	// Cambia la posicion de la entidad
	void mover(int x, int y);
	// Getters para la posicion
	int getX() const;
	int getY() const;
	// Actualiza las cosas internas, si hubiese
	void update();
	// Dibuja la entidad
	void blit(SDL_Surface* dest, const SDL_Rect& cam);

};