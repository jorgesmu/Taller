#include "ImagenPersonaje.h"

/**
	Pre: Condiciones sobre la entrada:
		
		altoSprite y anchoSprite: mayores iguales a cero, caso contrario se
		vuelca a default

		fps: mayor que cero, caso contario se vuelca a default

		delay: mayor o igual a cero, caso contrario se vuelca a 0.

		colorKey: cualquier entero para representar un RGB de 32 bits.

	Post: Si se logra abrir el archivo y tomar memoria, la instancia se
	inicializa de acuerdo a la imagen dada por el path.
	
	Nota: Setea en la accion EST_SUR.
**/
ImagenPersonaje::ImagenPersonaje(const char* name , const int altoSprite , 
				const int anchoSprite , const int fps , const int delay , ResMan& rm ,
				const int colorKey) : 
				ImagenAnimada(name ,altoSprite , anchoSprite , fps , delay , rm , colorKey) {
	this -> accionActual = ImagenPersonaje::EST_SUR;
	this -> setAccion(ImagenPersonaje::EST_SUR);
}

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
	
	Nota: Setea en la accion EST_SUR.
**/
ImagenPersonaje::ImagenPersonaje(const char* path , const int altoSprite , const int anchoSprite , 
				const int fps , const int delay , const int colorKey) : 
				ImagenAnimada(path , altoSprite , anchoSprite , fps , delay , colorKey) {
	this -> accionActual = ImagenPersonaje::EST_SUR;
	this -> setAccion(ImagenPersonaje::EST_SUR);
}


/**
	Pre: La instancia ha sido creada.

	Post: Se liberan los recursos asociados a la instancia.
**/
ImagenPersonaje::~ImagenPersonaje() {
	if (!(this -> compartida)) {	
		this -> surfaceOrigen -> destroy();
		delete(this -> surfaceOrigen);
		this -> surfaceOrigen = NULL;
	}
	this -> surfaceActual.destroy();
}

/*
	Pre: La instancia ha sido creada y el parametro es una accion entre 0 y 24.

	Post: Se ha seteado la accion.
*/
void ImagenPersonaje::setAccionEfectiva(unsigned int accion) {
	if ((accion < (unsigned int) this -> maxFilas) && (accion <= ImagenPersonaje::MUERTE)) {
		this -> accionSiguiente = accion;
		int delta = this -> accionSiguiente	- this -> accionActual;	
		if (delta != 0) {
			//Es desplazamiento
			if (this -> accionSiguiente < ImagenPersonaje::AT_SUR){
				//Ambos desplazamientos
				if (this -> accionActual < ImagenPersonaje::AT_SUR){
					if (delta > 0) {
						if (delta <=3) {
							this -> accionActual++;
						} else {
							if (this -> accionActual == DES_SUR) {
								this -> accionActual = DES_SUROESTE;
							} else {
								this -> accionActual--;
							}
						}
					} else {
						if (delta >=-3) {
							this -> accionActual--;
						} else {
							if (this -> accionActual == DES_SUROESTE) {
								this -> accionActual = DES_SUR;
							} else {
								this -> accionActual++;
							}
						}
					}
				} else {
					// Siguiente Desplazamiento y Actual Ataque
					if (this->accionActual < ImagenPersonaje::EST_SUR) {
						this -> accionActual-= 8;
						this -> columnaActual = 0;
					} else {
						//Siguiente Desplazamiento y Actual Estatico
						if (this->accionActual < ImagenPersonaje::MUERTE) {
							this -> accionActual-= 16;
							this -> columnaActual = 0;
						} else {
							//Siguiente Desplazamiento y Actual Muerte
							if ( this -> accionActual == ImagenPersonaje::MUERTE) {
								this -> accionActual = ImagenPersonaje::DES_NORTE;
								this -> columnaActual = 0;
							} else {
								//Siguiente Desplazamiento y Actual Defensa
								if ( (accionActual >= DEF_SUR) && (accionActual <= DEF_SUROESTE)){
									this -> accionActual-= 29;
									this -> columnaActual = 0;
								}
							}
						}
					}
				}
			} else {
				// Siguiente es Ataque
				if (this -> accionSiguiente < ImagenPersonaje::EST_SUR){
					// Siguiente es Ataque y Actual es Desplazamiento
					if (this -> accionActual < ImagenPersonaje::AT_SUR){
						this -> accionActual += 8;
						this -> columnaActual = 0;
					}
					else {
						// Siguiente Ataque y Actual Ataque
						if (this -> accionActual < ImagenPersonaje::EST_SUR) {
							if (delta > 0) {
								if (delta <=3) {
									this -> accionActual++;
								} else {
									if (this -> accionActual == AT_SUR) {
										this -> accionActual = AT_SUROESTE;
									} else {
										this -> accionActual--;
									}
								}
							} else {
								if (delta >=-3) {
									this -> accionActual--;
								} else {
									if (this -> accionActual == AT_SUROESTE) {
										this -> accionActual = AT_SUR;
									} else {
										this -> accionActual++;
									}
								}
							}
						} else {
							// Siguiente Ataque y Actual Estatico
							if (this -> accionActual < ImagenPersonaje::MUERTE) {
								this -> accionActual-= 8;
								this -> columnaActual = 0;
							// Siguiente Ataque y Actual Muerte
							} else {
								//Siguiente Ataque y Actual Muerte
								if ( this -> accionActual == ImagenPersonaje::MUERTE) {
									this -> accionActual = ImagenPersonaje::AT_NORTE;
									this -> columnaActual = 0;
								} else {
									//Siguiente Ataque y Actual Defensa
									if ( (accionActual >= DEF_SUR) && (accionActual <= DEF_SUROESTE)){
										this -> accionActual-= 21;
										this -> columnaActual = 0;
									}
								}
							}
						 }
					}
				} else {
					// Siguiente Estatico
					if (this -> accionSiguiente < ImagenPersonaje::MUERTE){
						// Siguiente Estatico actual Desplazamiento
						if (this -> accionActual < ImagenPersonaje::AT_SUR) {
							this -> accionActual += 16;
							this -> columnaActual = 0;
						} else {
							// Siguiente Estatico actual Ataque
							if (this -> accionActual < ImagenPersonaje::EST_SUR){
								this -> accionActual += 8;
								//this -> columnaActual = 0;
							} else {
								// Siguiente Estatico actual Estatico
								if (this -> accionActual < ImagenPersonaje::MUERTE) {
									if (delta > 0) {
										if (delta <=3) {
											this -> accionActual++;
										} else {
											if (this -> accionActual == EST_SUR) {
												this -> accionActual = EST_SUROESTE;
											} else {
												this -> accionActual--;
											}
										}
									} else {
									if (delta >=-3) {
										this -> accionActual--;
									} else {
										if (this -> accionActual == EST_SUROESTE) {
											this -> accionActual = EST_SUR;
										} else {
											this -> accionActual++;
										}
									}
								}										
								// Siguiente Estatico actual Muerte
								} else {
									this -> accionActual = ImagenPersonaje::EST_SUR;
									this -> columnaActual = 0;
								}
							}
						}
					// Siguiente Muerte
					} else {
						this -> accionActual = ImagenPersonaje::MUERTE;
						this -> columnaActual = 0;
					}
				}
			}
		}
		//seteo de fila
		this -> filaActual = this -> accionActual;
	}
}

/*
	Pre: La instancia ha sido creada y el parametro es una accion entre 0 y 24.

	Post: Se ha seteado la accion.
*/
bool ImagenPersonaje::setAccion(unsigned int accion){
	bool retorno = false;
	if (accion > ImagenPersonaje::MUERTE) {
		if (accion <= ImagenPersonaje::ESTATICO_DIRECCION_ACTUAL) {
			switch (accion) {
				case ImagenPersonaje::AVANCE_DIRECCION_ACTUAL : {
					// Accion actual no es desplazamiento
					if (this -> accionActual >= ImagenPersonaje::AT_SUR){
						// Accion actual ataque
						if (this -> accionActual < ImagenPersonaje::EST_SUR){
							this -> setAccionEfectiva(this -> accionActual - 8);
						} else {
							// Accion actual estatico 
							if (accionActual < MUERTE){
								this -> setAccionEfectiva( this -> accionActual-16);
							} else {
								// Si accion actual muerte
								if (accionActual == MUERTE){
									this -> setAccionEfectiva(ImagenPersonaje::DES_SUR);
								} else {
									// Si accion actual defensa
									if ( (accionActual >= DEF_SUR) && (accionActual <= DEF_SUROESTE)){
										this -> setAccionEfectiva(this -> accionActual-29);
									}
								}
							}
						}
					}
					break;
				}
				case ImagenPersonaje::ATAQUE_DIRECCION_ACTUAL : {
					// Accion actual desplazamiento
					if (this -> accionActual < ImagenPersonaje::AT_SUR){
						this -> setAccionEfectiva(this -> accionActual + 8);
					} else {
						// Accion actual estatico
						if ((this -> accionActual >= ImagenPersonaje::EST_SUR)
							&& (accionActual < ImagenPersonaje::MUERTE)) {
							this -> setAccionEfectiva(this -> accionActual-8);
						} else {
							// Accion actual muerte
							if (this -> accionActual == ImagenPersonaje::MUERTE){
								this -> setAccionEfectiva(ImagenPersonaje::AT_SUR);
							} else {
								// Accion actual defensa
								if ( (accionActual >= DEF_SUR) && (accionActual <= DEF_SUROESTE) ) {
									this -> setAccionEfectiva( this -> accionActual-21);
								}
							}
						}
					}
					break;
				}
				case ImagenPersonaje::ESTATICO_DIRECCION_ACTUAL : {
					// Accion actual desplazamiento
					if (this -> accionActual < ImagenPersonaje::AT_SUR){
						this -> setAccionEfectiva(this -> accionActual + 16);
					} else {
						// Accion actual ataque
						if (this -> accionActual < ImagenPersonaje::EST_SUR){
							this -> setAccionEfectiva(this -> accionActual + 8);
						} else {
							// Accion actual muerte
							if ( this -> accionActual == ImagenPersonaje::MUERTE) {
								this -> setAccionEfectiva(ImagenPersonaje::EST_SUR);
							} else {
								// Accion actual defensa
								if ( (this ->accionActual >= ImagenPersonaje::DEF_SUR) &&
									 (this ->accionActual <= ImagenPersonaje::DEF_SUROESTE)) {
									this -> setAccionEfectiva(this -> accionActual - 13);
								}
							}
						}
					}
					break;
				}
				case ImagenPersonaje::DEFENSA_DIRECCION_ACTUAL : {
					// Accion actual desplazamiento
					if (this -> accionActual < ImagenPersonaje::AT_SUR){
						this -> setAccionEfectiva(this -> accionActual + 29);
					} else {
						// Accion actual ataque
						if (this -> accionActual < ImagenPersonaje::EST_SUR){
							this -> setAccionEfectiva(this -> accionActual + 21);
						} else {
							// Accion actual estatico
							if ( this -> accionActual < ImagenPersonaje::MUERTE) {
								this -> setAccionEfectiva(this -> accionActual + 13);
							} else {
								// Accion actual muerte
								if ( this -> accionActual == ImagenPersonaje::MUERTE) {
									this -> setAccionEfectiva(ImagenPersonaje::DEF_SUR);
								}
							}
						}
					}
					break;
				}		
			}
		}	
	} else {
		this -> setAccionEfectiva(accion);
	}
	if (this -> columnaActual == this -> maxFilas) {
		retorno = true;
	}
	return retorno;
}

/*
	Pre: Se han inicilizado todos los parametros.

	Post: Se ha actualizado surfaceActual
*/
void ImagenPersonaje::nextSprite() {
	clock_t tiempoActual = clock();
	if (this -> tiempoProximoFrame <= tiempoActual) {
		this -> surfaceActual.nuevoSurfaceConfigurado(this -> getAlto() ,
					this -> getAncho() , SDL_GetVideoInfo() , colorKey);
		if ((this -> maxColumnas > 0) && (this -> maxFilas > 0) ) {
			this -> setAccion(this -> accionSiguiente);
			this -> columnaActual++;
			this -> tiempoProximoFrame += this -> deltaFrame;
			if (this -> columnaActual >= this -> maxColumnas) { 
				if ( (this -> accionActual >= ImagenPersonaje::AT_SUR) &&
					 (this -> accionActual <= ImagenPersonaje::AT_SUROESTE) ){
						 this -> accionSiguiente = this -> accionActual + 8;
				} else {
					if ( (this -> accionActual >= ImagenPersonaje::DEF_SUR) &&
					 (this -> accionActual <= ImagenPersonaje::DEF_SUROESTE) ){
						 this -> accionSiguiente = this -> accionActual - 13;
					}
				}
				this -> columnaActual = 0;
				this -> tiempoProximoFrame += this -> delay;
			}
			SDL_Rect rect;
			rect.h = this -> getAlto();
			rect.w = this ->getAncho();
			rect.x = this -> columnaActual * this -> getAncho();
			rect.y = this -> filaActual * this -> getAlto();
			if (this ->surfaceOrigen != NULL) {
				this -> surfaceOrigen -> blit( (this -> surfaceActual).getSDL_Surface() , 0 , 0, rect);
				this -> surfaceOrigen -> blitGris( (this -> surfaceActual).getSDL_SurfaceGris() , 0 , 0, rect);
			}
		}
	}
}

bool ImagenPersonaje::isImagenAnimada() {
	return false;
}

unsigned int ImagenPersonaje::getTipoAccion() {
	unsigned int retorno = ImagenPersonaje::ESTATICO_DIRECCION_ACTUAL;
	if (this -> accionActual < ImagenPersonaje::AT_SUR) {
		retorno = ImagenPersonaje::AVANCE_DIRECCION_ACTUAL;
	} else {
		if (this -> accionActual < ImagenPersonaje::EST_SUR) {
			retorno = ImagenPersonaje::ATAQUE_DIRECCION_ACTUAL;
		} else {
			if (this -> accionActual < ImagenPersonaje::MUERTE) {
				retorno = ImagenPersonaje::MUERTE;
			} else {
				if ( (this -> accionActual >= ImagenPersonaje::DEF_SUR) &&
					 (this -> accionActual <= ImagenPersonaje::DEF_SUROESTE) ){
					retorno = ImagenPersonaje::DEFENSA_DIRECCION_ACTUAL;
				}
			}
		}
	}
	return retorno;
}