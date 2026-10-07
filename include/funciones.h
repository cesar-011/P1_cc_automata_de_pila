#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include "PilaAutomata.h"
#include "APF.h"
#include "Estado.h"

#include <fstream>
#include <iostream>
#include <sstream>


void ConstruirAutomataDePila(std::ifstream& fichero_entrada, APF& automata, PilaAutomata& pila_automata);
bool ProcesarArgumentos(int argc, char* argv[], std::string& nombre_fichero, bool& traza_activada);
void EjecutarAutomata(APF& automata, PilaAutomata& pila_automata, bool traza_activada);

#endif //FUNCIONES_H_