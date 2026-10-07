#include "../include/APF.h"
#include <iostream>
#include <sstream>

/// @brief Método para cargar los estados del autómata
/// @param estados Conjunto de estados del autómata
void APF::CargarEstados(const std::set<Estado>& estados) {
  this->estados_ = estados;
}

/// @brief Método para cargar el alfabeto de la cadena
/// @param alfabeto Conjunto de símbolos del alfabeto de entrada
void APF::CargarAlfabeto(const std::set<std::string>& alfabeto) {
  this->alfabeto_ = alfabeto;
}

/// @brief Método para cargar el alfabeto de la pila
/// @param alfabeto_pila Conjunto de símbolos del alfabeto de la pila
void APF::CargarAlfabetoPila(const std::set<std::string>& alfabeto_pila) {
  this->alfabeto_pila_ = alfabeto_pila;
}

/// @brief Método para cargar el estado inicial del autómata
/// @param nombre_estado Nombre del estado a establecer como inicial
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

/// @brief Método para cargar el símbolo inicial de la pila
/// @param simbolo_inicial_pila Símbolo inicial con el que arranca la pila
void APF::CargarSimboloInicialPila(const std::string& simbolo_inicial_pila) {
  this->simbolo_inicial_pila_ = simbolo_inicial_pila;
}

/// @brief Método para cargar los estados finales del autómata
/// @param estados_finales Conjunto de estados de aceptación
void APF::CargarEstadosFinales(const std::set<Estado>& estados_finales) {
  this->estados_finales_ = estados_finales;
}

/// @brief Método para añadir una transición a la función de transición del autómata
/// @param estado_origen Estado de partida
/// @param sim_cadena Símbolo leído de la cadena
/// @param sim_pila Símbolo extraído de la pila
/// @param estado_destino Estado al que se transiciona
/// @param sim_pila_escribir Cadena de símbolos a insertar en la pila
void APF::AñadirTransicion(const std::string& estado_origen, 
                           const std::string& sim_cadena, 
                           const std::string& sim_pila, 
                           const std::string& estado_destino, 
                           const std::string& sim_pila_escribir) {
    
    //Empaquetamos la condición
    Condicion condicion = {sim_cadena, sim_pila};
    // Empaquetamos el destino 
    Destino destino = {estado_destino, sim_pila_escribir};
    this->funcion_transicion_[estado_origen][condicion].push_back(destino);
}

/// @brief Método para iniciar la evaluación de una cadena en el autómata
/// @param cadena Cadena de entrada a evaluar
/// @param pila_inicial Estado inicial de la pila
/// @param mostrar_traza Indica si se debe mostrar el seguimiento por consola
/// @return true si la cadena es aceptada, false en caso contrario
bool APF::EvaluarCadena(const std::string& cadena, PilaAutomata pila_inicial, bool mostrar_traza) const {
  if (mostrar_traza) {
    std::cout << "\n=== INICIANDO EVALUACION ===\n";
    std::cout << "Cadena a evaluar: '" << cadena << "'\n";
  }
  return EvaluarRama(cadena, this->estado_inicial_, pila_inicial, mostrar_traza);
}

/// @brief Método recursivo para evaluar una rama de exploración del autómata
/// @param cadena_restante Subcadena pendiente por procesar
/// @param estado_actual Estado actual del autómata en este paso
/// @param pila_actual Estado actual de la pila en este paso
/// @param mostrar_traza Indica si se debe mostrar la traza por consola
/// @return true si la rama actual alcanza un estado de aceptación, false si falla
bool APF::EvaluarRama(const std::string& cadena_restante, const Estado& estado_actual, PilaAutomata pila_actual, bool mostrar_traza) const {
  std::string nombre_origen = estado_actual.get_estado();

  if (mostrar_traza) {
    std::cout << "-> Estado: " << nombre_origen << " | Cadena Restante: '" << cadena_restante << "' | Pila: " << pila_actual.ToString() << "\n";
  }

  // Condición de aceptación 
  if (cadena_restante.empty()) {
    if (estados_finales_.find(estado_actual) != estados_finales_.end()) {
      if (mostrar_traza) std::cout << "  Cadena aceptada en estado final: " << nombre_origen << "\n";
      return true; 
    }
  }

  if (pila_actual.Empty()) return false;

  std::string sim_pila = pila_actual.Top();
  auto it_estado = funcion_transicion_.find(nombre_origen);
  if (it_estado == funcion_transicion_.end()) return false;
    
  const auto& mapa_destinos = it_estado->second;

  // Identificar transiciones épsilon/lambda aplicables
  auto it_epsilon = mapa_destinos.find({".", sim_pila});

  // Identificar transiciones por carácter aplicables
  std::string sim_cadena = "";
  bool tiene_caracter = !cadena_restante.empty();
  if (tiene_caracter) {
    sim_cadena = std::string(1, cadena_restante[0]);
  }
  auto it_caracter = tiene_caracter ? mapa_destinos.find({sim_cadena, sim_pila}) : mapa_destinos.end();

  // Mostrar la lista de posibles transiciones en el modo traza
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

/// @brief Método para comprobar la validez y coherencia formal del autómata
/// @return true si el autómata es formalmente válido, false si presenta errores
bool APF::ComprobarValidez() const {
  bool es_valido = true;

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


  // Comprobamos que el estado incial existe en los estados
  if (estados_.find(estado_inicial_) == estados_.end()) {
    std::cerr << "[ERROR] El estado inicial '" << estado_inicial_.get_estado() << "' no pertenece a Q.\n";
    es_valido = false;
  }

  // Comprobamos que el simbolo incial de la pila pertenece al alfabeto de la pila
  if (alfabeto_pila_.find(simbolo_inicial_pila_) == alfabeto_pila_.end()) {
    std::cerr << "[ERROR] El símbolo inicial de la pila '" << simbolo_inicial_pila_ << "' no está en Gamma.\n";
    es_valido = false;
  }

  // Comprobamos que los estados finales pertenecen a al conjunto de estados
  for (const auto& estado_final : estados_finales_) {
    if (estados_.find(estado_final) == estados_.end()) {
      std::cerr << "[ERROR] El estado final '" << estado_final.get_estado() << "' no pertenece a Q.\n";
      es_valido = false;
    }
  }

  // Validamos las transiciones
  for (const auto& [nombre_origen, mapa_destinos] : funcion_transicion_) {
    
    // Comprobamos que exista el estado de origen
    if (estados_.find({nombre_origen}) == estados_.end()) {
      std::cerr << "[ERROR] Transición inválida: El estado de origen '" << nombre_origen << "' no existe en Q.\n";
      es_valido = false;
    }

    for (const auto& [condicion, destinos] : mapa_destinos) {
      const std::string& sim_cadena = condicion.first;
      const std::string& sim_pila = condicion.second;

      // comprobamos que el simbolo de la cadena pertenece al alfabeto
      if (sim_cadena != "." && alfabeto_.find(sim_cadena) == alfabeto_.end()) {
        std::cerr << "[ERROR] Transición desde '" << nombre_origen 
                  << "': Símbolo de lectura '" << sim_cadena << "' no está en Sigma.\n";
        es_valido = false;
      }

      // Comprobamos que el simbolo que se va a extraer de la pila exista en el alfabeto de la pila
      if (alfabeto_pila_.find(sim_pila) == alfabeto_pila_.end()) {
        std::cerr << "[ERROR] Transición desde '" << nombre_origen 
                  << "': Símbolo de extracción de pila '" << sim_pila << "' no está en Gamma.\n";
        es_valido = false;
      }

      for (const auto& destino : destinos) {
        const std::string& nombre_destino = destino.first;
        const std::string& sim_pila_escribir = destino.second;

        // Imprimimos la transición
        std::cout << "  d(" << nombre_origen << ", " << sim_cadena << ", " << sim_pila 
                  << ") -> (" << nombre_destino << ", " << sim_pila_escribir << ")\n";
        
        // Comprobamso que exista el estado de destino
        if (estados_.find({nombre_destino}) == estados_.end()) {
          std::cerr << "[ERROR] Transición hacia estado inexistente: '" << nombre_destino << "'.\n";
          es_valido = false;
        }
        
        // Comprobamos que los simbolos a insertar en la pila pertenecen al alfabeto de la pila
        if (sim_pila_escribir != ".") {
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