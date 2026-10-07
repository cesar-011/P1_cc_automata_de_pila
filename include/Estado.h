#ifndef ESTADO_H_
#define ESTADO_H_

#include <string>

class Estado {
  public:
  Estado() {};
   Estado(const std::string& estado, bool inicial = false, bool final = false): estado_(estado), estado_inicial_(inicial), estado_final_(final) {}
   std::string get_estado() const { return this->estado_; }
   bool EsInicial() const { return estado_final_; }
   bool EsFinal() const { return estado_final_; }
   bool operator<(const Estado& otro_estado) const { return this->estado_ < otro_estado.get_estado(); }
   void set_inicial();
   Estado& operator=(const Estado& otro_estado);
   
  private:
   std::string estado_; // Nombre del estado
   bool estado_inicial_; // Si es estado incial o no
   bool estado_final_;// Si es estado final o no
};

#endif //ESTADO_H_