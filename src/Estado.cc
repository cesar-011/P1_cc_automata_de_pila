#include "../include/Estado.h"

void Estado::set_inicial() {
  this->estado_inicial_ = true;
}

Estado& Estado::operator=(const Estado& otro_estado) {
  if (this == &otro_estado) {
      return *this;
  }

  this->estado_ = otro_estado.get_estado();
  this->estado_inicial_ = otro_estado.EsInicial();
  this->estado_final_ = otro_estado.EsFinal();
  
  return *this; 
}