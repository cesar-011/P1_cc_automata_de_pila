#include "../include/funciones.h"

/// @brief Lee la configuración desde un fichero de entrada e inicializa el autómata y su pila
/// @param fichero_entrada Flujo de entrada del archivo de configuración
/// @param automata Referencia al objeto APF donde se estructurará el autómata
/// @param pila_automata Referencia a la estructura de pila a configurar
void ConstruirAutomataDePila(std::ifstream& fichero_entrada, APF& automata, PilaAutomata& pila_automata) {
  std::string linea;
  int iterator = 0;
  if (fichero_entrada.is_open()) {
    while (std::getline(fichero_entrada, linea)) {
      // Ignorar comentarios
      if (linea.empty() || linea[0] == '#') {
        continue;
      }
      switch (iterator) {
        case 0: {
          // Guardar estados
          std::stringstream line_string(linea);
          std::string estado_string;
          std::set<Estado> estados;
          while (line_string >> estado_string) {
            Estado estado(estado_string);
            estados.insert(estado);
          }
          automata.CargarEstados(estados);
          break;
        }
        case 1: {
          // Guardar alfabeto cadena
          std::stringstream line_string(linea);
          std::string simbolo;
          std::set<std::string> alfabeto;
          while (line_string >> simbolo) {
            alfabeto.insert(simbolo);
          }
          automata.CargarAlfabeto(alfabeto);
          break;
        }
        case 2: {
          // Guardar alfabeto pila
          std::stringstream line_string(linea);
          std::string simbolo;
          std::set<std::string> alfabeto_pila;
          while (line_string >> simbolo) {
            alfabeto_pila.insert(simbolo);
          }
          automata.CargarAlfabetoPila(alfabeto_pila);
          pila_automata.set_alfabeto(alfabeto_pila);
          break;
        }
        case 3: {
          // Guardar estado incial
          std::stringstream line_string(linea);
          std::string estado_inicial;
          line_string >> estado_inicial;
          
          automata.CargarEstadoInicial(estado_inicial);
          break;
        }
        case 4: {
          // Guardar simbolo inicial de la pila
          std::stringstream line_string(linea);
          std::string simbolo_inicial;
          line_string >> simbolo_inicial;
          
          pila_automata.CargarSimboloInicial(simbolo_inicial);
          automata.CargarSimboloInicialPila(simbolo_inicial);
          break;
        }
        case 5: {
          // Guardar estados finales
          std::stringstream line_string(linea);
          std::string estado_string;
          std::set<Estado> estados_finales;
          while (line_string >> estado_string) {
            Estado estado(estado_string);
            estados_finales.insert(estado);
          }
          automata.CargarEstadosFinales(estados_finales);
          break;
        }
        case 6: {
          // Mapear funcion de transicion
          std::stringstream line_string(linea);
          std::string estado_origen, sim_cadena, sim_pila, estado_destino, sim_pila_escribir;
          // Extraemos las 5 palabras de la línea
          if (line_string >> estado_origen >> sim_cadena >> sim_pila >> estado_destino >> sim_pila_escribir) {
              automata.AñadirTransicion(estado_origen, sim_cadena, sim_pila, estado_destino, sim_pila_escribir);
          }
          continue;
        }
      
        default:
          break;
      }
      ++iterator;
    }

    fichero_entrada.close();
    } else {
      std::cerr << "Error: No se pudo abrir el archivo." << '\n';
    }
}

/// @brief Analiza los argumentos pasados por línea de comandos
/// @param argc Número de argumentos pasados al programa
/// @param argv Vector de cadenas con los argumentos de la línea de comandos
/// @param nombre_fichero Referencia donde se almacenará el nombre del archivo de configuración
/// @param traza_activada Referencia donde se almacenará si el modo traza está activado
/// @return true si los argumentos son válidos, false en caso contrario
bool ProcesarArgumentos(int argc, char* argv[], std::string& nombre_fichero, bool& traza_activada) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-config") {
      if (i + 1 < argc) {
        nombre_fichero = argv[++i]; 
      } else {
        std::cerr << "Error: La opción -config requiere el nombre de un fichero.\n";
        return false;
      }
    } else if (arg == "-trace") {
      traza_activada = true;
    } else {
      std::cerr << "Opción desconocida: " << arg << "\n";
      std::cerr << "Uso: " << argv[0] << " -config <fichero> [-trace]\n";
      return false;
    }
  }

  if (nombre_fichero.empty()) {
    std::cerr << "Error: Es obligatorio especificar un fichero de configuración.\n";
    std::cerr << "Uso: " << argv[0] << " -config <fichero> [-trace]\n";
    return false;
  }

  return true;
}

/// @brief Gestiona el bucle interactivo de la consola para leer y evaluar cadenas
/// @param automata Referencia al autómata de pila a utilizar
/// @param pila_automata Referencia al estado inicial de la pila del autómata
/// @param traza_activada Indica si se debe mostrar la traza durante la evaluación
void EjecutarAutomata(APF& automata, PilaAutomata& pila_automata, bool traza_activada) {
  std::string cadena;
  std::cout << "\nAutómata listo. Introduce cadenas para evaluar (escribe 'salir' para terminar):\n";
  std::cout << "(Usa '.' para representar la cadena vacía)\n";

  while (true) {
    std::cout << "> ";
    std::cin >> cadena;

    if (cadena == "salir") break;
    if (cadena == ".") cadena = ""; 

    if (automata.EvaluarCadena(cadena, pila_automata, traza_activada)) {
      std::cout << "  [RESULTADO] La cadena es ACEPTADA.\n\n";
    } else {
      std::cout << "  [RESULTADO] La cadena es RECHAZADA.\n\n";
    }
  }
}