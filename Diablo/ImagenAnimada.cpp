#include "ImagenAnimada.h"

/**
	Pre: Condiciones sobre la entrada:
		
		path: no nulo, caso contrario se vuelca a NULL

		altoSprite y anchoSprite: mayores iguales a cero, caso contrario se
		vuelca a default

		fps: mayor que cero, caso contario se vuelca a default

		delay: mayor o igual a cero, caso contrario se vuelca a 0.

		colorKey: cualquier entero para representar un RGB de 32 bits.

	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	inicializa de acuerdo a la imagen dada por el path.
**/
ImagenAnimada::ImagenAnimada(const char* path , const int altoSprite , const int anchoSprite , 
				const int fps , const int delay , const int colorKey){
	
	//seteo de dimensiones sprite
	if (altoSprite >= 0){
		this -> setAlto(altoSprite);
	} else{
		this -> setAlto(Imagen::ALTO_DEFAULT);
	}
	if (anchoSprite >= 0){
		this -> setAncho(anchoSprite);
	} else{
		this -> setAlto(Imagen::ANCHO_DEFAULT);
	}
	
	//seteo de tiempos de delay y fps
	if (delay >= 0) {
		this -> delay = delay;
	} else {
		this -> delay = ImagenAnimada::DELAY_DEFAULT;
	}
	if (fps > 0) {
		this -> fps = fps;
	} else {
		this -> fps = ImagenAnimada::FPS_DEFAULT;
	}
	
	//seteo la distancia entre frames
	this -> deltaFrame = 1000 / this -> fps ;
	
	//calculo de tiempos
	clock_t tiempoActual = clock();
	this -> tiempoProximoFrame = tiempoActual + deltaFrame;
	
	//lectura de imagen de origen
	if (this -> surfaceOrigen.load(path , colorKey)){
		this -> setPath(path);
		//calculo cantidad de filas y columnas
		if (this-> getAncho() <= this -> surfaceOrigen.width()) {
			this -> maxColumnas = this -> surfaceOrigen.width() / this -> getAncho();
		} else {
			this -> setAncho(this -> surfaceOrigen.width());
			this -> maxColumnas = 1;
		}
		if (this-> getAlto() <= this -> surfaceOrigen.height()) {
			this -> maxFilas = this -> surfaceOrigen.height() / this -> getAlto();
		} else {
			this -> setAlto(this -> surfaceOrigen.height());
			this -> maxFilas = 1;
		}		
	} else {
		this -> setPath(NULL);
		this -> maxFilas = 0;
		this -> maxColumnas = 0;
	}	
	this -> filaActual = 0;
	this -> columnaActual = -1;
	this -> colorKey = colorKey;
}

/**
	Pre: La instancia ha sido creada.

	Post: Se liberan los recursos asociados a la instancia.
**/
ImagenAnimada::~ImagenAnimada() {
	delete[] this -> getPath();
	this -> setPath(NULL);
	this -> surfaceOrigen.destroy();
	this -> surfaceActual.destroy();
}

/*
	Pre: Se han inicilizado todos los parámetros.

	Post: Se ha actualizado surfaceActual
*/
void ImagenAnimada::nextSprite() {
	clock_t tiempoActual = clock();
	if (this -> tiempoProximoFrame <= tiempoActual) {
		this -> surfaceActual.nuevoSurfaceConfigurado(this -> getAlto() ,
					this -> getAncho() , SDL_GetVideoInfo() , colorKey);
		if ((this -> maxColumnas > 0) && (this -> maxFilas > 0) ) {
			this -> columnaActual++;
			this -> tiempoProximoFrame += this -> deltaFrame;
			if (this -> columnaActual >= this -> maxColumnas) {
				this -> columnaActual = 0;
				this -> filaActual++;
				if (this -> filaActual >= this -> maxFilas) {
					this -> filaActual = 0;
				}
				this -> tiempoProximoFrame += this -> delay;
			}
			SDL_Rect rect;
			rect.h = this -> getAlto();
			rect.w = this ->getAncho();
			rect.x = this -> columnaActual * this -> getAncho();
			rect.y = this -> filaActual * this -> getAlto();
			this -> surfaceOrigen.blit( (this -> surfaceActual).getSDL_Surface() , 0 , 0, rect);
		}
	}
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna un puntero a una imagen en memoria de vídeo, cuya destrucción
	es responsabilidad de esta instancia.
		
**/
Surface* ImagenAnimada::getSurface() {
	this -> nextSprite();
	return & (this -> surfaceActual);
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna la cantidad de FPS.
		
**/
int ImagenAnimada::getFPS() {
	return this -> fps;
}

/**
	Pre: La instancia ha sido creada.

	Post: Se retorna el Delay.
		
**/
int ImagenAnimada::getDelay() {
	return this -> delay;
}