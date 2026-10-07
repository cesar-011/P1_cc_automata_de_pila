#ifndef PILA_AUTOMATA_H_
#define PILA_AUTOMATA_H_

#include <stack>
#include <set>
#include <string>

class PilaAutomata {
  public:
   PilaAutomata() {}
   void set_alfabeto(const std::set<std::string>& alfabeto_pila_);
   void CargarSimboloInicial(const std::string& simbolo_inicial);
   bool Empty() const { return pila_.empty(); }
   std::string Top() const { return pila_.top(); }
   void Pop() { if (!pila_.empty()) pila_.pop(); }
   void Push(const std::string& simbolo) { pila_.push(simbolo); }
   std::string ToString() const;

  private:
   std::stack<std::string> pila_; // Pila del automata
   std::set<std::string> alfabeto_pila_; // Alfabeto de la pila
};

#endif //PILA_AUTOMATA_H_