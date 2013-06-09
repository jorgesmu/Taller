#include "ImagenArma.h"

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
ImagenArma::ImagenArma(const char* name , const int fps , const int delay , ResMan& rm ,
				const int colorKey) : ImagenAnimada(name , ImagenArma::ALTO_DEFAULT , 
				ImagenArma::ANCHO_DEFAULT , fps , delay , rm , colorKey) {
	this -> accionActual = ImagenArma::EST_SUR;
	this -> setAccion(ImagenArma::INVISIBLE);
}

/**
	Pre: La instancia ha sido creada.

	Post: Se liberan los recursos asociados a la instancia.
**/
ImagenArma::~ImagenArma() {
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
void ImagenArma::setAccionEfectiva(unsigned int accion) {
	if ((accion < (unsigned int) this -> maxFilas) && (accion <= ImagenArma::ITEM)) {
		this -> accionSiguiente = accion;
		int delta = this -> accionSiguiente	- this -> accionActual;	
		if (delta != 0) {
			//Es desplazamiento
			if (this -> accionSiguiente < ImagenArma::AT_SUR){
				//Ambos desplazamientos
				if (this -> accionActual < ImagenArma::AT_SUR){
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
					if (this->accionActual < ImagenArma::EST_SUR) {
						this -> accionActual-= 8;
						this -> columnaActual = 0;
					} else {
						//Siguiente Desplazamiento y Actual Estatico
						if (this->accionActual <= ImagenArma::EST_SUROESTE) {
							this -> accionActual-= 16;
							this -> columnaActual = 0;
						} else {
							//Siguiente Desplazamiento y Actual Accion Especial
							this -> accionActual = ImagenArma::DES_NORTE;
							this -> columnaActual = 0;
						}
					}
				}
			} else {
				// Siguiente es Ataque
				if (this -> accionSiguiente < ImagenArma::EST_SUR){
					// Siguiente es Ataque y Actual es Desplazamiento
					if (this -> accionActual < ImagenArma::AT_SUR){
						this -> accionActual += 8;
						this -> columnaActual = 0;
					}
					else {
						// Siguiente Ataque y Actual Ataque
						if (this -> accionActual < ImagenArma::EST_SUR) {
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
							// Siguiente Ataque y actual Actual Estatico
							if (this -> accionActual < ImagenArma::ITEM) {
								this -> accionActual-= 8;
								this -> columnaActual = 0;
							// Siguiente Ataque y actual Accion Especial
							} else {
								this -> accionActual = ImagenArma::AT_NORTE;
								this -> columnaActual = 0;
							}
						 }
					}
				} else {
					// Siguiente Estatico
					if (this -> accionSiguiente < ImagenArma::ITEM){
						// Siguiente Estatico actual Desplazamiento
						if (this -> accionActual < ImagenArma::AT_SUR) {
							this -> accionActual += 16;
							this -> columnaActual = 0;
						} else {
							// Siguiente Estatico actual Ataque
							if (this -> accionActual < ImagenArma::EST_SUR){
								this -> accionActual += 8;
								//this -> columnaActual = 0;
							} else {
								// Siguiente Estatico actual Estatico
								if (this -> accionActual < ImagenArma::ITEM) {
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
								// Siguiente Estatico actual Accion Especial
								} else {
									this -> accionActual = ImagenArma::EST_SUR;
									this -> columnaActual = 0;
								}
							}
						}
					} else {
						if (this -> accionActual == ImagenArma::ITEM) {
							this -> accionActual = ImagenArma::ITEM;
							this -> columnaActual = 0;
						} else {
							this -> accionActual = accion;
							this -> filaActual = accion;
						}
					}
				}
			}
		}
		//seteo de fila
		this -> filaActual = this -> accionActual;
	} else {
		this -> accionSiguiente = ImagenArma::INVISIBLE;
		this -> accionActual = ImagenArma::INVISIBLE;
		//seteo de fila
		this -> filaActual = this -> accionActual;
	}
}

/*
	Pre: La instancia ha sido creada y el parametro es una accion entre 0 y 24.

	Post: Se ha seteado la accion.
*/
bool ImagenArma::setAccion(unsigned int accion){
	bool retorno = false;
	if (accion > ImagenArma::INVISIBLE) {
		if (accion < ImagenArma::CONTINUAR_CON_ACCION_ACTUAL) {
			switch (accion) {
				case ImagenArma::AVANCE_DIRECCION_ACTUAL : {
					// Accion actual no es desplazamiento
					if (this -> accionActual >= ImagenArma::AT_SUR){
						// Accion actual ataque
						if (this -> accionActual < ImagenArma::EST_SUR){
							this -> setAccionEfectiva(this -> accionActual - 8);
						} else {
							// Accion actual estatico 
							if (accionActual < ITEM){
								this -> setAccionEfectiva( this -> accionActual-16);
							} else {
								// Si accion actual ACCION ESPECIAL
								this -> setAccionEfectiva(ImagenArma::DES_SUR);
								
							}
						}
					}
					break;
				}
				case ImagenArma::ATAQUE_DIRECCION_ACTUAL : {
					// Accion actual desplazamiento
					if (this -> accionActual < ImagenArma::AT_SUR){
						this -> setAccionEfectiva(this -> accionActual + 8);
					} else {
						// Accion actual estatico
						if ((this -> accionActual >= ImagenArma::EST_SUR)
							&& (accionActual < ImagenArma::ITEM)) {
							this -> setAccionEfectiva(this -> accionActual-8);
						} else {
							// Accion actual ACCION ESPECIAL
							this -> setAccionEfectiva(ImagenArma::AT_SUR);
						}
					}
					break;
				}
				case ImagenArma::ESTATICO_DIRECCION_ACTUAL : {
					// Accion actual desplazamiento
					if (this -> accionActual < ImagenArma::AT_SUR){
						this -> setAccionEfectiva(this -> accionActual + 16);
					} else {
						// Accion actual ataque
						if (this -> accionActual < ImagenArma::EST_SUR){
							this -> setAccionEfectiva(this -> accionActual + 8);
						} else {
							// Accion actual ACCION ESPECIAL
							this -> setAccionEfectiva(ImagenArma::EST_SUR);
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
void ImagenArma::nextSprite() {
	clock_t tiempoActual = clock();
	if (this -> tiempoProximoFrame <= tiempoActual) {
		this -> surfaceActual.nuevoSurfaceConfigurado(this -> getAlto() ,
					this -> getAncho() , SDL_GetVideoInfo() , colorKey);
		if ((this -> maxColumnas > 0) && (this -> maxFilas > 0) ) {
			this -> setAccion(this -> accionSiguiente);
			clock_t deltaTiempo = tiempoActual - this -> tiempoProximoFrame + deltaFrame;
			this -> columnaActual  = columnaActual + ceil((double)deltaTiempo * fpc);
			this -> tiempoProximoFrame = tiempoActual + this -> deltaFrame;
			// Ataques o defesas vuelven a su estatico correspondiente
			if (this -> columnaActual >= this -> maxColumnas) { 
				if ( (this -> accionActual >= ImagenArma::AT_SUR) &&
						(this -> accionActual <= ImagenArma::AT_SUROESTE) ){
							this -> accionSiguiente = INVISIBLE;
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

bool ImagenArma::isImagenAnimada() {
	return false;
}
