#include "../include/APF.h"
#include <iostream>
#include <sstream>


void APF::CargarEstados(const std::set<Estado>& estados) {
  this->estados_ = estados;
}

void APF::CargarAlfabeto(const std::set<std::string>& alfabeto) {
  this->alfabeto_ = alfabeto;
}

void APF::CargarAlfabetoPila(const std::set<std::string>& alfabeto_pila) {
  this->alfabeto_pila_ = alfabeto_pila;
}

void APF::CargarEstadoInicial(const std::string& nombre_estado) {
  // Buscamos el estado incial en los estados y lo marcamos como incial
  // Buscamos el estado inicial en el set
  auto it = this->estados_.find({nombre_estado});
  if (it != this->estados_.end()) {
    // Lo extraemos
    auto estado = this->estados_.extract(it);
    // Lo modificamos
    estado.value().set_inicial();
    this->estado_inicial_ = estado.value();
    // Lo volvemos a insertar
    this->estados_.insert(std::move(estado));
  } else {
    std::cerr << "Error: El estado inicial '" << nombre_estado << "' no ha sido definido previamente en los estados.\n";
    throw std::runtime_error("Estado inicial no encontrado");
  }
}

void APF::CargarSimboloInicialPila(const std::string& simbolo_inicial_pila) {
  this->simbolo_inicial_pila_ = simbolo_inicial_pila;
}

void APF::CargarEstadosFinales(const std::set<Estado>& estados_finales) {
  this->estados_finales_ = estados_finales;
}

void APF::AñadirTransicion(const std::string& estado_origen, 
                           const std::string& sim_cadena, 
                           const std::string& sim_pila, 
                           const std::string& estado_destino, 
                           const std::string& sim_pila_escribir) {
    
    //Empaquetamos la condición (Lo que leo)
    Condicion condicion = {sim_cadena, sim_pila};
    // Empaquetamos el destino (A dónde voy y qué escribo)
    Destino destino = {estado_destino, sim_pila_escribir};
    this->funcion_transicion_[estado_origen][condicion].push_back(destino);
}




bool APF::EvaluarCadena(const std::string& cadena, PilaAutomata pila_inicial, bool mostrar_traza) const {
  if (mostrar_traza) {
    std::cout << "\n=== INICIANDO EVALUACION ===\n";
    std::cout << "Cadena a evaluar: '" << cadena << "'\n";
  }
  return EvaluarRama(cadena, this->estado_inicial_, pila_inicial, mostrar_traza);
}

bool APF::EvaluarRama(const std::string& cadena_restante, const Estado& estado_actual, PilaAutomata pila_actual, bool mostrar_traza) const {
  std::string nombre_origen = estado_actual.get_estado();

  if (mostrar_traza) {
    std::cout << "-> Estado: " << nombre_origen << " | Cadena Restante: '" << cadena_restante << "' | Pila: " << pila_actual.ToString() << "\n";
  }

  // Condición de aceptación (por estado final)
  if (cadena_restante.empty()) {
    if (estados_finales_.find(estado_actual) != estados_finales_.end()) {
      if (mostrar_traza) std::cout << "  [!] Cadena aceptada en estado final: " << nombre_origen << "\n";
      return true; 
    }
  }

  if (pila_actual.Empty()) return false;

  std::string sim_pila = pila_actual.Top();
  auto it_estado = funcion_transicion_.find(nombre_origen);
  if (it_estado == funcion_transicion_.end()) return false;
    
  const auto& mapa_destinos = it_estado->second;

  // 1. Identificar transiciones épsilon/lambda aplicables
  auto it_epsilon = mapa_destinos.find({".", sim_pila});

  // 2. Identificar transiciones por carácter aplicables (si la cadena no está vacía)
  std::string sim_cadena = "";
  bool tiene_caracter = !cadena_restante.empty();
  if (tiene_caracter) {
    sim_cadena = std::string(1, cadena_restante[0]);
  }
  auto it_caracter = tiene_caracter ? mapa_destinos.find({sim_cadena, sim_pila}) : mapa_destinos.end();

  // 3. Mostrar la lista de posibles transiciones en el modo traza
  if (mostrar_traza) {
    std::cout << "  Posibles transiciones aplicables:\n";
    bool hay_opciones = false;

    if (it_epsilon != mapa_destinos.end()) {
      for (const auto& destino : it_epsilon->second) {
        std::cout << "    * d(" << nombre_origen << ", ., " << sim_pila 
                  << ") -> (" << destino.first << ", " << destino.second << ")\n";
        hay_opciones = true;
      }
    }

    if (it_caracter != mapa_destinos.end()) {
      for (const auto& destino : it_caracter->second) {
        std::cout << "    * d(" << nombre_origen << ", " << sim_cadena << ", " 
                  << sim_pila << ") -> (" << destino.first << ", " << destino.second << ")\n";
        hay_opciones = true;
      }
    }

    if (!hay_opciones) {
      std::cout << "    * (Ninguna transición disponible)\n";
    }
  }

  // RAMA A: Transiciones lambda/épsilon (".")
  if (it_epsilon != mapa_destinos.end()) {
    for (const auto& destino : it_epsilon->second) {
      if (mostrar_traza) {
        std::cout << "  Aplicando transicion: d(" << nombre_origen << ", ., " 
                  << sim_pila << ") -> (" << destino.first << ", " << destino.second << ")\n";
      }
            
      PilaAutomata pila_rama = pila_actual; 
      pila_rama.Pop(); 
            
      if (destino.second != ".") {
        for (int i = destino.second.length() - 1; i >= 0; --i) {
          pila_rama.Push(std::string(1, destino.second[i]));
        }
      }

      if (EvaluarRama(cadena_restante, Estado(destino.first), pila_rama, mostrar_traza)) {
        return true; 
      }
    }
  }

  // RAMA B: Transiciones consumiendo caracter
  if (it_caracter != mapa_destinos.end()) {
    for (const auto& destino : it_caracter->second) {
      if (mostrar_traza) {
        std::cout << "  Aplicando transicion: d(" << nombre_origen << ", " << sim_cadena << ", " 
                  << sim_pila << ") -> (" << destino.first << ", " << destino.second << ")\n";
      }
                
      PilaAutomata pila_rama = pila_actual; 
      pila_rama.Pop();
                
      if (destino.second != ".") {
        for (int i = destino.second.length() - 1; i >= 0; --i) {
          pila_rama.Push(std::string(1, destino.second[i]));
        }
      }

      if (EvaluarRama(cadena_restante.substr(1), Estado(destino.first), pila_rama, mostrar_traza)) {
        return true;
      }
    }
  }

  if (mostrar_traza) std::cout << "  [X] Camino sin salida, retrocediendo...\n";
  return false;
}













bool APF::ComprobarValidez() const {
    bool es_valido = true;

    // =========================================================
    // PARTE 1: MOSTRAR LA DEFINICIÓN DEL AUTÓMATA (PRINT)
    // =========================================================
    std::cout << "\n=== COMPROBACIÓN DEL AUTÓMATA ===\n";
    
    std::cout << "Estados (Q): { ";
    for (const auto& estado : estados_) std::cout << estado.get_estado() << " ";
    std::cout << "}\n";

    std::cout << "Alfabeto de la cadena (Sigma): { ";
    for (const auto& sim : alfabeto_) std::cout << sim << " ";
    std::cout << "}\n";

    std::cout << "Alfabeto de la pila (Gamma): { ";
    for (const auto& sim : alfabeto_pila_) std::cout << sim << " ";
    std::cout << "}\n";

    std::cout << "Estado inicial (q0): " << estado_inicial_.get_estado() << "\n";
    std::cout << "Símbolo inicial de la pila (Z0): " << simbolo_inicial_pila_ << "\n";

    std::cout << "Estados finales (F): { ";
    for (const auto& estado_final : estados_finales_) std::cout << estado_final.get_estado() << " ";
    std::cout << "}\n";

    std::cout << "---------------------------------\n";
    std::cout << "Función de transición (delta):\n";

    // =========================================================
    // PARTE 2: VALIDACIÓN RIGUROSA Y MUESTRA DE TRANSICIONES
    // =========================================================

    // 1. Comprobar estado inicial
    if (estados_.find(estado_inicial_) == estados_.end()) {
        std::cerr << "[ERROR] El estado inicial '" << estado_inicial_.get_estado() << "' no pertenece a Q.\n";
        es_valido = false;
    }

    // 2. Comprobar símbolo inicial de la pila
    if (alfabeto_pila_.find(simbolo_inicial_pila_) == alfabeto_pila_.end()) {
        std::cerr << "[ERROR] El símbolo inicial de la pila '" << simbolo_inicial_pila_ << "' no está en Gamma.\n";
        es_valido = false;
    }

    // 3. Comprobar estados finales
    for (const auto& estado_final : estados_finales_) {
        if (estados_.find(estado_final) == estados_.end()) {
            std::cerr << "[ERROR] El estado final '" << estado_final.get_estado() << "' no pertenece a Q.\n";
            es_valido = false;
        }
    }

    // 4. Validar e imprimir Transiciones
    for (const auto& [nombre_origen, mapa_destinos] : funcion_transicion_) {
        
        // A) ¿El estado de origen existe?
        if (estados_.find({nombre_origen}) == estados_.end()) {
            std::cerr << "[ERROR] Transición inválida: El estado de origen '" << nombre_origen << "' no existe en Q.\n";
            es_valido = false;
        }

        for (const auto& [condicion, destinos] : mapa_destinos) {
            const std::string& sim_cadena = condicion.first;
            const std::string& sim_pila = condicion.second;

            // B) ¿El símbolo leído es válido? (Asumo '.' como épsilon)
            if (sim_cadena != "." && alfabeto_.find(sim_cadena) == alfabeto_.end()) {
                std::cerr << "[ERROR] Transición desde '" << nombre_origen 
                          << "': Símbolo de lectura '" << sim_cadena << "' no está en Sigma.\n";
                es_valido = false;
            }

            // C) ¿El símbolo extraído de la pila es válido?
            if (alfabeto_pila_.find(sim_pila) == alfabeto_pila_.end()) {
                std::cerr << "[ERROR] Transición desde '" << nombre_origen 
                          << "': Símbolo de extracción de pila '" << sim_pila << "' no está en Gamma.\n";
                es_valido = false;
            }

            for (const auto& destino : destinos) {
                const std::string& nombre_destino = destino.first;
                const std::string& sim_pila_escribir = destino.second;

                // Imprimimos la transición con formato matemático clásico: d(q0, a, Z) -> (q1, AZ)
                std::cout << "  d(" << nombre_origen << ", " << sim_cadena << ", " << sim_pila 
                          << ") -> (" << nombre_destino << ", " << sim_pila_escribir << ")\n";
                
                // D) ¿El estado de destino existe?
                if (estados_.find({nombre_destino}) == estados_.end()) {
                    std::cerr << "[ERROR] Transición hacia estado inexistente: '" << nombre_destino << "'.\n";
                    es_valido = false;
                }
                
                // E) ¿Los símbolos a insertar en la pila son válidos?
                // Verificamos si no es épsilon ('.'). Si vas a insertar cosas, iteramos letra por letra.
                if (sim_pila_escribir != ".") {
                    // Asumimos que cada caracter del string representa 1 símbolo de la pila
                    for (char caracter : sim_pila_escribir) {
                        std::string simbolo_individual(1, caracter);
                        if (alfabeto_pila_.find(simbolo_individual) == alfabeto_pila_.end()) {
                            std::cerr << "[ERROR] En transición hacia '" << nombre_destino 
                                      << "': El símbolo a insertar '" << simbolo_individual 
                                      << "' no pertenece a Gamma.\n";
                            es_valido = false;
                        }
                    }
                }
            }
        }
    }

    std::cout << "=================================\n";
    if (es_valido) {
        std::cout << "[OK] El autómata se ha construido correctamente y es matemáticamente coherente.\n\n";
    } else {
        std::cerr << "[FALLO] El autómata contiene inconsistencias formales.\n\n";
    }

    return es_valido;
}