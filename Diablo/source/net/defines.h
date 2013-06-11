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
//Actualizacion de la velocidad
const unsigned char UPDATE_ATT = 'x';
// Velocidad que manda cuando se conecta un usuario existente
const unsigned char OLD_ATT = 'y';
// Atributos del jugador que se acaba de conectar para el resto de los clientes
const unsigned char INIT_ATT = 'z';
// Congela a un personaje
const unsigned char CONGELAR = 'A';
// Revive posicion
const unsigned char REQUEST_REV_POS = 'B';
// Revive posicion OK
const unsigned char POS_REQUEST_REV_REPLY = 'C';
// Transportar player a revive posicion
const unsigned char REV_PLAYER = 'D';
// Colocar bandera en una posicion del mapa
const unsigned char NEW_FLAG = 'E';
// Atrape una bandera
const unsigned char CATCH_FLAG = 'F';
// Aviso al clliente que gano la mision
const unsigned char WINNER = 'G';
//Avisa que murio enemigo
const unsigned char ENEMY_DEAD = 'H';
//avise si termine de mover
const unsigned char EN_MOVE_CMPLT = 'I';
// Exploto una bomba
const unsigned char BOMB_OFF = 'J';
// uso golem
const unsigned char USO_GOLEM = 'K';
// Descongelo un personaje
const unsigned char DESCONGELAR = 'L';
// Transmutacion - Convirtio a un enemigo en otra cosa
const unsigned char TRANSMUT = 'M';
// Vuevlo atras la transmutacion
const unsigned char DESTRANSMUT = 'N';


}

// Cosas de movimiento
namespace MOV {
	const int IDLE = 0;
	const int ESPERANDO_OK = 1;
	const int OK_RECV = 2;
	const int FAIL_RECV = 3;
	const int MANDAR_POS = 4;
	const int MUERTO = 5;
	const int OK_REV_RECV = 6;
}

// Cosas de items
namespace ITEM {
	const char TERREMOTO = 0;
	const char HIELO = 1;
	const char BOMBA = 2;
}

// Cosas de atributos
namespace ATT {
	const char VEL = 0;
	const char ENERGIA = 1;
	const char MAGIA = 2;
	const char ENERGIA_ESCUDO = 3;
	const char CONGELADO = 4;
	const char CANT_TERREMOTO = 5;
	const char CANT_HIELO = 6;
	const char RADIO = 7;
	const char BOLA_DE_CRISTAL = 8;
	const char GOLEM = 9;
}

// Cosas a las que se puede transmutar
namespace TIPO {
	const char LAPIDA = 0;
	const char ESTRATEGIA_ENEMY = 1;
}