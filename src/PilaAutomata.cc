#include "../include/PilaAutomata.h"

void PilaAutomata::set_alfabeto(const std::set<std::string>& alfabeto) {
  this->alfabeto_pila_ = alfabeto;
}

void PilaAutomata::CargarSimboloInicial(const std::string& simbolo_inicial) {
  this->pila_.push(simbolo_inicial);
}