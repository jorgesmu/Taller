#pragma once

// Constantes para el protocolo

namespace PROTO {

// Mensaje de tipo chat: <str:destino_nick><str:mensaje>
const unsigned char CHAT = 'a';

// Mensaje de set de nick: <str:nuevo_nick>
const unsigned char NICK = 'b';

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

}