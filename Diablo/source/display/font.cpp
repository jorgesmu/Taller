#include "font.h"
#include <iostream>
#include <cassert>

// Ctor y destructor
Font::Font() {
	font = NULL;
}

Font::~Font() {
	// Como no es seguro liberar recursos aca, lo unico que hacemos es verificar que se hayan liberado
	if(font != NULL || surf_buffer.size() > 0) {
		std::cerr << "Error liberando recursos en Font::~Font\n";
	}
}

// Inicializa el font dado el .ttf y un tamaño, devuelve false en caso de error
bool Font::init(const std::string& font_file, int size) {
	// Cargamos
	font = TTF_OpenFont(font_file.c_str(), size);
	if(font == NULL) {
		std::cerr << "Error loading font <" << font_file << ">: " << TTF_GetError() << "\n";
		return false;
	}else{
		return true;
	}
}

// Limpia todos los recursos
void Font::clean() {
	// Liberamos el font
	TTF_CloseFont(font);
	font = NULL;
	// Liberamos todas las surfaces buffereadas
	for(auto it = surf_buffer.begin();it != surf_buffer.end();it++) {
		SDL_FreeSurface(it->second);
	}
	surf_buffer.clear();
}

// Se entiende
void Font::blit(SDL_Surface* dest, int x, int y, const std::string& msg, const SDL_Color& color) {
	// Cargamos las coordenadas a donde blittear
	SDL_Rect dest_coords; dest_coords.x = x; dest_coords.y = y;
	SDL_Surface* txt_surf = TTF_RenderText_Solid(this->font, msg.c_str(), color);
	assert(txt_surf != NULL);
	SDL_BlitSurface(txt_surf, NULL, dest, &dest_coords);
	SDL_FreeSurface(txt_surf);
}

// Se entiende
void Font::blitCentered(SDL_Surface* dest, int x, int y, const std::string& msg, const SDL_Color& color) {
	// Cargamos las coordenadas a donde blittear
	SDL_Rect dest_coords; dest_coords.x = x; dest_coords.y = y;
	SDL_Surface* txt_surf = TTF_RenderText_Solid(this->font, msg.c_str(), color);
	assert(txt_surf != NULL);
	// Centramos
	dest_coords.x -= txt_surf->w/2;
	dest_coords.y -= txt_surf->h/2;
	SDL_BlitSurface(txt_surf, NULL, dest, &dest_coords);
	SDL_FreeSurface(txt_surf);
}

// Esta funcion hace el blit pero guarda una copia del surface la primera vez
// Si ese surface ya existe, se lo reutiliza
void Font::buffBlit(SDL_Surface* dest, int x, int y, const std::string& msg, const SDL_Color& color) {
	SDL_Rect dest_coords; dest_coords.x = x; dest_coords.y = y;
	// Primero nos fijamos si este msg+color ya fue renderizado
	const std::string hash = genBlitHash(msg, color);
	auto it = surf_buffer.find(hash);
	if(it != surf_buffer.end()) {
		// Blitteo buffereado
		SDL_BlitSurface(it->second, NULL, dest, &dest_coords);
	}else{
		// Generamos el surface
		SDL_Surface* txt_surf = TTF_RenderText_Solid(this->font, msg.c_str(), color);
		assert(txt_surf != NULL);
		// Insertamos en el buffer
		surf_buffer[hash] = txt_surf;
		// Blitteamos
		SDL_BlitSurface(txt_surf, NULL, dest, &dest_coords);
	}
}

// Genera un hash dado un string y su color
std::string Font::genBlitHash(const std::string& msg, const SDL_Color& color) {
	std::string hash = msg;
	hash.append(1, char(color.r));
	hash.append(1, char(color.g));
	hash.append(1, char(color.b));
	return hash;
}

void Font::calcSize(const std::string& msg, int* w, int* h) const {
	if(TTF_SizeText(this->font, msg.c_str(), w, h) == -1) {
		std::cerr << "Error @ Font::calcSize(): " << TTF_GetError() << "\n";
	}
}

int Font::getH() const {
	return TTF_FontHeight(this->font);
}