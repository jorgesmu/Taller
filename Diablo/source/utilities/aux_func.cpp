#include "aux_func.h"
#include <fstream>
#include <windows.h>

SDL_Rect makeRect(int x, int y) {
	SDL_Rect r;
	r.x = x;
	r.y = y;
	return r;
}

SDL_Rect makeRect(int x, int y, int w, int h) {
	SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return r;
}

int intRand(int min, int max) {
    return min + (rand() % (int)(max - min + 1));
}

float fRand(float v1, float v2) {
    return v1 + (v2-v1)*((float)rand())/((float)RAND_MAX);
}

size_t fileSize(const std::string& file) {
    std::ifstream in(file, std::ifstream::in | std::ifstream::binary);
	if(in.bad())
		return 0;
    in.seekg(0, std::ifstream::end);
    return in.tellg(); 
}

void listFilesinDir(std::string dir, std::vector<std::string>& ret) {
	HANDLE hFind;
	WIN32_FIND_DATA data;

	dir.append("\\*.*");

	hFind = FindFirstFile(dir.c_str(), &data);
	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			if(strlen(data.cFileName) > 3)
				ret.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}