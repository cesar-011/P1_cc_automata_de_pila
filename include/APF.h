#ifndef APF_H_
#define APF_H_

#include "Estado.h"
#include "PilaAutomata.h"

#include <set>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <fstream>

class APF {
  public:
   APF() {};
   void CargarEstados(const std::set<Estado>& estados);
   void CargarAlfabeto(const std::set<std::string>& alfabeto);
   void CargarAlfabetoPila(const std::set<std::string>& alfabeto_pila);
   void CargarEstadoInicial(const std::string& linea);
   void CargarSimboloInicialPila(const std::string& simbolo_inicial_pila);
   void CargarEstadosFinales(const std::set<Estado>& estados_finales);
   void AñadirTransicion(const std::string& estado_origen, 
                         const std::string& sim_cadena, 
                         const std::string& sim_pila, 
                         const std::string& estado_destino, 
                         const std::string& sim_pila_escribir);
   bool ComprobarValidez() const;

   bool EvaluarCadena(const std::string& cadena, PilaAutomata pila_inicial, bool mostrar_traza = false) const;
   bool EvaluarRama(const std::string& cadena_restante, const Estado& estado_inicial, PilaAutomata pila_automata, bool mostrar_traza) const;


  private:
   //Lo que leo: Pair(simbolo_cadena, simbolo_pila)
   using Condicion = std::pair<std::string, std::string>; 
   //A dónde voy: Pair(nuevo_estado, simbolos_a_escribir)
   using Destino = std::pair<std::string, std::string>;   
   //El mapa interior: De una condición a múltiples destinos (No determinismo)
   using MapaDestinos = std::map<Condicion, std::vector<Destino>>; 
   //La estructura final
   using FuncionTransicion = std::map<std::string, MapaDestinos>;

   std::set<Estado> estados_;
   std::set<std::string> alfabeto_;
   std::set<std::string> alfabeto_pila_;
   Estado estado_inicial_;
   std::string simbolo_inicial_pila_;
   std::set<Estado> estados_finales_;
   FuncionTransicion funcion_transicion_;
};

#endif //APF_H_