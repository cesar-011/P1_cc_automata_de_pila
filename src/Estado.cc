#include "../include/Estado.h"

/// @brief Metodo para indicar que un estado es inicial
void Estado::set_inicial() {
  this->estado_inicial_ = true;
}

/// @brief Sobrecarga del operador = para asignar estados
/// @param otro_estado 
/// @return 
Estado& Estado::operator=(const Estado& otro_estado) {
  if (this == &otro_estado) {
    return *this;
  }

  this->estado_ = otro_estado.get_estado();
  this->estado_inicial_ = otro_estado.EsInicial();
  this->estado_final_ = otro_estado.EsFinal();
  
  return *this; 
}