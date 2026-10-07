#include "../include/funciones.h"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  std::string nombre_fichero = "";
  bool traza_activada = false;

  // Analizo los argumentos de la línea de comandos
  if (!ProcesarArgumentos(argc, argv, nombre_fichero, traza_activada)) {
    return 1;
  }

  // Abro el fichero
  std::ifstream fichero_entrada(nombre_fichero);
  if (!fichero_entrada.is_open()) {
    std::cerr << "Error al abrir el fichero: " << nombre_fichero << "\n";
    return 1;
  }

  APF automata;
  PilaAutomata pila_automata;
  
  // Construyo el automata y la pila del automata
  std::cout << "Construyendo autómata...\n";
  ConstruirAutomataDePila(fichero_entrada, automata, pila_automata);
  
  // Compruebo la validez del automata
  if (!automata.ComprobarValidez()) {
    std::cerr << "Autómata mal formado.\n";
    return 1;
  }

  // Ejecuto el automata para validar la cadena
  EjecutarAutomata(automata, pila_automata, traza_activada);

  return 0;
}