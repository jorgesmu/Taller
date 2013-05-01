#pragma once

// Constantes para el protocolo

namespace PROTO {

// Mensaje de tipo chat: <str:destino_nick><str:mensaje>
const unsigned char CHAT = 'a';

// Mensaje de set de nick: <str:nuevo_nick>
const unsigned char NICK = 'b';

// Mensaje de sincronizacion inicial: ???
const unsigned char FIRST_SYNC = 'c';

// Mensaje de envio de archivos: <cant_archivos>
const unsigned char FILE_SEND = 'd';

// Mensaje de texto de parte del server
const unsigned char TEXTMSG = 'e';

}