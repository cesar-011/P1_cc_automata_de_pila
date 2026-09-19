#ifndef APF_H_
#define APF_H_

#include <set>
#include <map>
#include <string>
#include <vector>
#include <utility>

class APF {
  public:
   
  private:
   //Lo que leo: Pair(simbolo_cadena, simbolo_pila)
   using Condicion = std::pair<std::string, std::string>; 
   //A dónde voy: Pair(nuevo_estado, simbolos_a_escribir)
   using Destino = std::pair<std::string, std::string>;   
   //El mapa interior: De una condición a múltiples destinos (No determinismo)
   using MapaDestinos = std::map<Condicion, std::vector<Destino>>; 
   //La estructura final
   using FuncionTransicion = std::map<std::string, MapaDestinos>;

   std::set<std::string> estados_;
   std::set<std::string> alfabeto_;
   std::set<std::string> alfabeto_pila_;
   std::string estado_inicial_;
   std::string simbolo_inicial_pila_;
   std::set<std::string> estados_finales_;
   FuncionTransicion funcion_transicion_;
};

#endif //APF_H_