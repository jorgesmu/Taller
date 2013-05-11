#pragma once

#include "sinput.h"

#include <deque>
#include <string>

// Fwd Decl.
class ResMan;

class ChatWindow {
	private:
		// Lista de mensajes
		std::deque<std::string> msg_queue;
		int max_msg_count, max_msg_width; // Cantidad de lineas y cantidad de chars horizontales
		int h, w; // Ancho y alto en pixels
		int x, y; // Posicion absoluta en la pantalla, en pixels
		int char_w, char_h; // La altura de un solo caracter
		int font_size;
		SDL_Color font_color;
		// Link con ResMan*
		ResMan* rm;
		SInput input_box;
		bool showing;
		std::string nick_local, nick_destino;
	public:
		// Ctor y Dtor
		ChatWindow();
		~ChatWindow();
		// Metodos
		// Constructor, tamaño en pixels, font en Font::SIZE_*
		void init(ResMan* rm, int x, int y, int font_size, int height, int width, const SDL_Color& color);
		int handleInput(SDL_Event &event);
		// Muestra el input en la pantalla
		void show(SDL_Surface* dest);
		// Control
		void addMsg(const std::string& nick, const std::string& msg);
		bool isOpen() const;
		void hide();
		void open();
		void setNickLocal(const std::string& nick);
		void setNickDestino(const std::string& nick);
};