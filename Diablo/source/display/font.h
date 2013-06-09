#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <map>
#include <string>

class Font {
	private:
	// Puntero a la fuente
	TTF_Font* font;
	// Buffer para blitteo
	std::map<std::string, SDL_Surface*> surf_buffer;
	// Funcion de hash para el blitteo buffereado, es una mezcla del string y el color
	std::string genBlitHash(const std::string& msg, const SDL_Color& color);

	public:
	// Variables de clase para los 3 tamaños de fonts
	const static int SIZE_NORMAL = 12;
	const static int SIZE_SMALL = 10;
	const static int SIZE_BIG = 16;
	// Ctor y destructor
	Font();
	~Font();
	// Inicializa el font dado el .ttf y un tamaño, devuelve false en caso de error
	bool init(const std::string& font_file, int size);
	// Limpia todos los recursos
	void clean();
	// Se entiende
	void blit(SDL_Surface* dest, int x, int y, const std::string& msg, const SDL_Color& color);
	void blitCentered(SDL_Surface* dest, int x, int y, const std::string& msg, const SDL_Color& color);
	// Esta funcion hace el blit pero guarda una copia del surface la primera vez
	// Si ese surface ya existe, se lo reutiliza
	void buffBlit(SDL_Surface* dest, int x, int y, const std::string& msg, const SDL_Color& color);

	// Getters para ancho y alto de un mensaje
	// Sirve para formattear antes de imprimir
	void calcSize(const std::string& msg, int* w, int* h) const;
	// Calcula el alto maximo
	int getH() const;

};
