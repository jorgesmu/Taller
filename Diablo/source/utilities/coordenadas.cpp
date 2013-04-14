#include "coordenadas.h"
#include "logErrores.h"
#include <sstream>

vec2<int> tileWalk(const vec2<int>& current, int dir) {
	switch(dir) {
		case GDIR::N:
			return current + vec2<int>(-1,-1);
		case GDIR::NO:
			return current + vec2<int>( 0,-1);
		case GDIR::O:
			return current + vec2<int>( 1,-1);
		case GDIR::SO:
			return current + vec2<int>( 1, 0);
		case GDIR::S:
			return current + vec2<int>( 1, 1);
		case GDIR::SE:
			return current + vec2<int>( 0, 1);
		case GDIR::E:
			return current + vec2<int>(-1, 1);
		case GDIR::NE:
			return current + vec2<int>(-1, 0);
		// En caso de error
		default: {
			std::stringstream ss;
			ss << "Error, requested tileWalk with invalid direction (" << dir << ")\n";
			err_log.escribir(ss.str());
			return current;
		}
	}
}