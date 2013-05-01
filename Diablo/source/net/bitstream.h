#pragma once

#include <vector>
#include "SDL.h"

class BitStream {
    public:
        std::vector<unsigned char> buffer;
        Uint16 cpos;
        void clear();
        //cons
        BitStream();
        ~BitStream();
        BitStream(const char* load, const int size);
        const unsigned char* data(); // returns pointer to first elem in vector
		std::string str(); // Returns data in std::string
        size_t size() const;
        // overloads
        BitStream& operator <<(const Uint32 &val);
        BitStream& operator >>(Uint32 &val);
        BitStream& operator <<(const Uint16 &val);
        BitStream& operator >>(Uint16 &val);
        BitStream& operator <<(const Uint8 &val);
        BitStream& operator >>(Uint8 &val);
        BitStream& operator <<(const Sint32 &val);
        BitStream& operator >>(Sint32 &val);
        BitStream& operator <<(const Sint16 &val);
        BitStream& operator >>(Sint16 &val);
        BitStream& operator <<(const Sint8 &val);
        BitStream& operator >>(Sint8 &val);
        BitStream& operator <<(const char &val);
        BitStream& operator >>(char &val);
        BitStream& operator <<(const float &val);
        BitStream& operator >>(float &val);
        BitStream& operator <<(const std::string &s);
        BitStream& operator >>(std::string &s);
};

