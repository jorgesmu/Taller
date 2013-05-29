#pragma once

// Constantes para el protocolo

namespace PROTO {

// Mensaje de tipo chat: <str:destino_nick><str:mensaje>
const unsigned char CHAT = 'a';

// Mensaje de set de nick: <str:nuevo_nick>
const unsigned char NICKANDTYPE = 'b';

// Mensaje de texto de parte del server
const unsigned char TEXTMSG = 'c';

// Mensaje de envio de archivos: <cant_archivos>
const unsigned char FILE_SEND_COUNT = 'd';
// Mensaje de header de envio de archivo: <nombre_archivo>
const unsigned char FILE_HEADER = 'e';
// Mensaje de envio de parte de archivo: <tamaño data><data>
const unsigned char FILE_PART = 'f';
// Mensaje de fin de envio archivo
const unsigned char FILE_DONE = 'g';

// Mensaje de sincronizacion de tipo de personaje
const unsigned char PREVIOUSTYPE = 'h';

// Mensaje de sincronizacion de tipo default de personaje
const unsigned char DEFTYPE = 'i';

// Mensaje de sincronizacion de posicion inicial de personaje
const unsigned char INITPOS = 'j';

// Mensaje de sincronizacion de id escenario
const unsigned char ESC_ID = 'k';

// Mensajes de sincronizacion de nieble
const unsigned char NIEBLA_SYNC = 'm'; // cliente
const unsigned char NIEBLA_LIST = 'n'; // servidor

// Mensajes de sincronizacion de jugadores
// Sintaxis: <nick:string><tipo:string><x:int><y:int><gris:bool>
const unsigned char NEW_PLAYER = 'o'; // Informa al cliente que agregue un jugador nuevo
// Sintax: <nick:string>
const unsigned char PLAYER_EXIT = 'p'; // Informa al cliente que se desconecto un jugador

// <x:int><y:int>
const unsigned char REQUEST_POS = 'q';
// <bool:ok/not ok>
const unsigned char POS_REQUEST_REPLY = 'r';

// <nick:string><x:int><y:int>
const unsigned char MOVE_PLAYER = 's';

// Acciones
// <nick:string>
const unsigned char ATACAR = 't';
const unsigned char DEFENDER = 'u';

// Dañar
// <string:nick_who><string:nick_to><char:dmg>
const unsigned char DAMAGE = 'v';
// Usar item
// <nick:who><char:item>
const unsigned char USE_ITEM = 'w';
// Aumentar velocidad
// <nick:who><double:aumento>
const unsigned char ADD_VEL = 'x';

}

// Cosas de movimiento
namespace MOV {
	const int IDLE = 0;
	const int ESPERANDO_OK = 1;
	const int OK_RECV = 2;
	const int FAIL_RECV = 3;
	const int MANDAR_POS = 4;
}

// Cosas de items
namespace ITEM {
	const char TERREMOTO = 0;
	const char HIELO = 1;
}