#include "chatwindow.h"
#include "../display/resman.h"
#include "../utilities/aux_func.h"

#include <sstream>

const int VERTICAL_SPACING = 4;
const int VERTICAL_MARGIN = 5;
const int HORIZONTAL_MARGIN = 10;
const int CLOSE_BUTTON_SIDE = 25;

// Ctor y Dtor
ChatWindow::ChatWindow() {
	rm = NULL;
}

ChatWindow::~ChatWindow() {
}

// Metodos
void ChatWindow::init(ResMan* rm, int x, int y, int font_size, int height, int width, const SDL_Color& color) {
	// Ctor
	this->font_size = font_size;
	this->h = height;
	this->w = width;
	this->rm = rm;
	this->x = x;
	this->y = y;
	this->font_color = color;
	// Calculamos char_w y char_h, y el max_msg_count y max_msg_width
	rm->getFont(font_size)->calcSize("a", &char_w, &char_h);
	max_msg_width = std::floor(float(this->w - 2.0*HORIZONTAL_MARGIN)/char_w);
	max_msg_count = std::ceil(float(this->h - 2.0*VERTICAL_MARGIN - 2.0*char_h)/char_h) - 2;

	// Init al inputbox
	input_box.init(rm, font_size, max_msg_width, color);
	showing = false;

}

int ChatWindow::handleInput(SDL_Event &event) {
	// Control del input de texto
	int res = input_box.handleInput(event);
	if(res == SInput::RES_CLOSE) {
		this->hide();
	}else if(res == SInput::RES_ENTER) {
		// En vez de nicklocal iria personajes.getLocal().getNombre() o algo asi
		this->addMsg(nick_local, input_box.getText());
	}
	// Control del input del mouse
	if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		if(		event.button.x < x + this->w &&
				event.button.x > x + this->w - CLOSE_BUTTON_SIDE &&
				event.button.y > y &&
				event.button.y < y + CLOSE_BUTTON_SIDE)
		{
			res = SInput::RES_CLOSE;
		}
		
	}
	return res;
}

// Muestra el input en la pantalla
void ChatWindow::show(SDL_Surface* dest) {
	// Solo dibujamos si esta abierta
	if(!isOpen()) return;

	SDL_Rect rect;
	// Background
	rect.x = x-1; rect.y = y-1; rect.w = this->w+2; rect.h = this->h+2;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, COLOR::GRAY.r, COLOR::GRAY.g, COLOR::GRAY.b));
	rect.x = x; rect.y = y; rect.w = this->w; rect.h = this->h;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, COLOR::BLACK.r, COLOR::BLACK.g, COLOR::BLACK.b));
	// Lineas separadora
	rect.x = x; rect.y = y + this->h - 2*VERTICAL_MARGIN - this->char_h; rect.w = this->w; rect.h = 1;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, COLOR::GRAY.r, COLOR::GRAY.g, COLOR::GRAY.b));
	rect.x = x; rect.y = y + 2*VERTICAL_MARGIN + this->char_h; rect.w = this->w; rect.h = 1;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, COLOR::GRAY.r, COLOR::GRAY.g, COLOR::GRAY.b));
	// Boton de close
	rect.x = x+this->w - CLOSE_BUTTON_SIDE; rect.y = y; rect.w = CLOSE_BUTTON_SIDE; rect.h = CLOSE_BUTTON_SIDE;
	SDL_FillRect(dest, &rect, SDL_MapRGB(dest->format, COLOR::GRAY.r, COLOR::GRAY.g, COLOR::GRAY.b));
	rm->getFont(Font::SIZE_BIG)->buffBlit(dest, x+this->w - 0.75*CLOSE_BUTTON_SIDE, y, "X", COLOR::WHITE);
	// Titulo
	rm->getFont()->blit(dest, x+HORIZONTAL_MARGIN, y+VERTICAL_MARGIN, std::string("Chat con <")+nick_destino+std::string(">"), COLOR::WHITE);
	// Entrada de texto
	input_box.show(dest, x + HORIZONTAL_MARGIN, y + this->h - VERTICAL_MARGIN - this->char_h);

	// Mensajes ingresados
	int start_x = x + HORIZONTAL_MARGIN;
	int start_y = y + 3*VERTICAL_MARGIN + char_h;
	for(auto it = msg_queue.begin();it != msg_queue.end();it++) {
		rm->getFont()->blit(dest, start_x, start_y, *it, COLOR::WHITE);
		start_y += char_h;
	}
}

// Control
void ChatWindow::addMsg(const std::string& nick, const std::string& msg) {
	// Formateamos y agregamos el mensaje
	std::stringstream ss;
	ss << "<" << nick << "> " << msg;
	std::string res = ss.str();
	if(res.size() > max_msg_width) {
		while(res.size() > max_msg_width) {
			const int delta = res.size() - max_msg_width;
			msg_queue.push_back(res.substr(0, max_msg_width));
			res.erase(0, max_msg_width);
		}
		msg_queue.push_back(res);
	}else{
		msg_queue.push_back(res);
	}
	while(msg_queue.size() > this->max_msg_count) {
		msg_queue.pop_front();
	}
}

bool ChatWindow::isOpen() const {
	return showing;
}

void ChatWindow::hide() {
	// Borramos todo lo que habia en el "historial" onClose
	msg_queue.clear();
	showing = false;
	input_box.hide();
}

void ChatWindow::open() {
	showing = true;
	input_box.open();
}

void ChatWindow::setNickLocal(const std::string& nick) {
	nick_local = nick;
}

void ChatWindow::setNickDestino(const std::string& nick) {
	nick_destino = nick;
}