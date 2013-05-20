#include "bitstream.h"
#include "SDL.h"
#include "winsock2.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// Numero de posiciones decimales que se transmiten en los float
const int PRECISION = 1000;

BitStream::BitStream() {
    cpos = 0;
    buffer.clear();
}

BitStream::BitStream(const char* load, size_t size) {
	this->load(load, size);
}

void BitStream::load(const char* load, size_t size) {
	cpos = 0;
	buffer.resize(size);
    std::copy(load, load+size, buffer.begin());
}

void BitStream::clear() {
    buffer.clear();
    cpos = 0;
}

BitStream::~BitStream() {
    clear();
}

const char* BitStream::data() {
    return &buffer[0];
}

size_t BitStream::size() const {
    return buffer.size();
}

std::string BitStream::str() {
	return std::string(buffer.begin(), buffer.end());
}

BitStream &BitStream::operator <<(const Uint32 &val){
	const char* c = reinterpret_cast<const char*>(&val);
    buffer.insert(buffer.end(), c, c+sizeof(Uint32));
    return *this;
}

BitStream &BitStream::operator >>(Uint32 &val){
    static unsigned char buf[sizeof(Uint32)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Uint32), buf);
    val = *(reinterpret_cast<Uint32*>(&buf));
    cpos += sizeof(Uint32);
    return *this;
}

BitStream &BitStream::operator <<(const Uint16& val){
	const char* c = reinterpret_cast<const char*>(&val);
    buffer.insert(buffer.end(), c, c+sizeof(Uint16));
    return *this;
}

BitStream &BitStream::operator >>(Uint16& val){
    static unsigned char buf[sizeof(Uint16)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Uint16), buf);
    val = *(reinterpret_cast<Uint16*>(&buf));
    cpos += sizeof(Uint16);
    return *this;
}

BitStream &BitStream::operator <<(const Uint8 &val){
    static unsigned char buf[sizeof(Uint8)];
    buf[0] = val;
    buffer.insert(buffer.end(), &val, &val+sizeof(Uint8));
    return *this;
}

BitStream &BitStream::operator >>(Uint8 &val){
    static unsigned char buf[sizeof(Uint8)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Uint8), buf);
    val = Uint8(buf[0]);
    cpos += sizeof(Uint8);
    return *this;
}

BitStream &BitStream::operator <<(const char &val){
    static unsigned char buf[sizeof(char)];
    buf[0] = val;
    buffer.insert(buffer.end(), buf, buf+sizeof(char));
    return *this;
}

BitStream &BitStream::operator >>(char &val){
    static unsigned char buf[sizeof(char)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(char), buf);
    val = char(buf[0]);
    cpos += sizeof(char);
    return *this;
}

BitStream &BitStream::operator <<(const bool &val){
    static unsigned char buf[sizeof(char)];
    buf[0] = (val ? 1 : 0);
    buffer.insert(buffer.end(), buf, buf+sizeof(char));
    return *this;
}

BitStream &BitStream::operator >>(bool &val){
    static unsigned char buf[sizeof(char)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(char), buf);
    val = (char(buf[0]) == 1 ? true : false);
    cpos += sizeof(char);
    return *this;
}

BitStream &BitStream::operator <<(const float &val){
    Sint32 tmp = Sint32( val * float(PRECISION) );
	const char* c = reinterpret_cast<char*>(&tmp);
    buffer.insert(buffer.end(), c, c+sizeof(Sint32));
    return *this;
}

BitStream &BitStream::operator >>(float &val){
    static unsigned char buf[sizeof(Sint32)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Sint32), buf);
	Sint32 tmp = *(reinterpret_cast<Sint32*>(&buf));
    val = float(Sint32(tmp)) / float(PRECISION);
    cpos += sizeof(Sint32);
    return *this;
}

BitStream &BitStream::operator <<(const std::string &s){
	short len = s.size();
	*this << len;
    buffer.insert(buffer.end(), s.begin(), s.end());
    return *this;
}
#include <exception>
BitStream &BitStream::operator >>(std::string &s){
    short len;
	*this >> len;
    s.resize(len);
	std::copy(buffer.begin()+cpos, buffer.begin()+cpos+len, s.begin());
    cpos += len;
    return *this;
}

BitStream &BitStream::operator <<(const Sint32 &val){
	const char* c = reinterpret_cast<const char*>(&val);
    buffer.insert(buffer.end(), c, c+sizeof(Sint32));
    return *this;
}

BitStream &BitStream::operator >>(Sint32 &val){
    static unsigned char buf[sizeof(Sint32)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Sint32), buf);
    val = *(reinterpret_cast<Sint32*>(&buf));
    cpos += sizeof(Sint32);
    return *this;
}

BitStream &BitStream::operator <<(const Sint16 &val){
	const char* c = reinterpret_cast<const char*>(&val);
    buffer.insert(buffer.end(), c, c+sizeof(Sint16));
    return *this;
}

BitStream &BitStream::operator >>(Sint16 &val){
    static unsigned char buf[sizeof(Sint16)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Sint16), buf);
    val = *(reinterpret_cast<Sint16*>(&buf));
    cpos += sizeof(Sint16);
    return *this;
}

BitStream &BitStream::operator <<(const Sint8 &val){
    static unsigned char buf[sizeof(Sint8)];
    buf[0] = val;
    buffer.insert(buffer.end(), buf, buf+sizeof(Sint8));
    return *this;
}

BitStream &BitStream::operator >>(Sint8 &val){
    static unsigned char buf[sizeof(Sint8)];
    std::copy(buffer.begin()+cpos, buffer.begin()+cpos+sizeof(Sint8), buf);
    val = Sint8(buf[0]);
    cpos += sizeof(Sint8);
    return *this;
}