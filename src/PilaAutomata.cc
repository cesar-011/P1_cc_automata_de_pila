#include "../include/PilaAutomata.h"

/// @brief Meotod para cargar el alfabeto de la pila
/// @param alfabeto 
void PilaAutomata::set_alfabeto(const std::set<std::string>& alfabeto) {
  this->alfabeto_pila_ = alfabeto;
}

/// @brief Metodo para cargar el simbolo incial en la pila
/// @param simbolo_inicial 
void PilaAutomata::CargarSimboloInicial(const std::string& simbolo_inicial) {
  this->pila_.push(simbolo_inicial);
}

/// @brief Metodo para mostrar la pila
/// @return Los simbolos de la pila en un string
std::string PilaAutomata::ToString() const {
  if (pila_.empty()) return "Vacia";
  std::stack<std::string> temp = pila_;
  std::string res = "";
  while (!temp.empty()) {
    res += temp.top();
    temp.pop();
  }
  return res;
}