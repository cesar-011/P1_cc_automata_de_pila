# Simulador de Autómata de Pila (APF)

Este proyecto implementa un simulador de **Autómatas de Pila por estado final (Pushdown Automata)** en C++. Es capaz de procesar autómatas deterministas y no deterministas, soportando transiciones vacías (épsilon/lambda) y ofreciendo un modo "traza" para visualizar paso a paso el proceso de evaluación de una cadena. 

## Compilación y Ejecución

```bash
make
./build/automata
```


### Uso

El simulador se ejecuta por línea de comandos y requiere un fichero de configuración. Opcionalmente, puedes activar el modo traza.

```bash
./simulador_apf -config <docs/fichero_configuracion> [-trace]
```

* **`-config <fichero>`**: (Obligatorio) Ruta al archivo de texto que define formalmente el autómata de pila.
* **`-trace`**: (Opcional) Muestra por consola los detalles internos (estado, cadena restante, pila) y los caminos explorados durante la evaluación de la cadena.

Una vez ejecutado, el programa entrará en un modo interactivo pidiendo cadenas a evaluar. 
* Usa `.` para representar la **cadena vacía**.
* Escribe `salir` para terminar la ejecución.

---

## Estructura del Código y Clases

El proyecto está dividido de forma modular, aplicando principios de Programación Orientada a Objetos:

### 1. Clase `APF` (`APF.h` / `APF.cc`)
Es el **motor principal** del simulador. Representa la tupla matemática del autómata de pila.
* **Almacena:** Estados, alfabeto de entrada, alfabeto de la pila, estado inicial, símbolo inicial de la pila, estados finales y la función de transición.
* **Función de Transición:** Implementada mediante mapas anidados (`std::map<std::string, std::map<Condicion, std::vector<Destino>>>`). Permite transiciones no deterministas al almacenar un vector de destinos posibles para cada par *(símbolo de cadena, símbolo de pila)*.
* **`EvaluarCadena()` / `EvaluarRama()`:** Funciones recursivas que evalúan si la cadena es aceptada. Al ser recursivas (Backtracking), son capaces de explorar todos los caminos posibles generados por el no determinismo y las transiciones épsilon.
* **`ComprobarValidez()`:** Analiza la coherencia matemática del autómata, validando que todos los símbolos y estados de las transiciones pertenezcan a los alfabetos y conjuntos previamente declarados.

### 2. Clase `Estado` (`Estado.h` / `Estado.cc`)
Representa un estado individual dentro del autómata.
* Almacena el nombre (etiqueta) del estado (ej: `q0`, `q1`) y banderas booleanas indicando si es inicial o final.
* Sobrecarga el operador `<` para poder ser insertado correctamente en estructuras ordenadas como `std::set`.

### 3. Clase `PilaAutomata` (`PilaAutomata.h` / `PilaAutomata.cc`)
Encapsula el comportamiento de la pila (LIFO) basándose en `std::stack<std::string>`.
* **Funciones Clave:** `Push`, `Pop`, `Top`, y `Empty`.
* **`ToString()`:** Extrae temporalmente los elementos para devolver una representación visual en forma de texto, muy útil para el modo `-trace`.

### 4. Módulo de Funciones Auxiliares (`funciones.h` / `funciones.cc`)
Contiene la lógica de inicialización y flujo del programa, dejando `main.cc` muy limpio.
* **`ConstruirAutomataDePila()`:** Analiza (parsea) secuencialmente el archivo de configuración línea por línea (saltando comentarios `#`) para cargar las piezas dentro del objeto `APF`.
* **`ProcesarArgumentos()`:** Valida las opciones y parámetros de la consola.
* **`EjecutarAutomata()`:** Mantiene el bucle interactivo pidiendo al usuario cadenas (hasta que escribe `salir`).

---

## Formato del Archivo de Configuración

El archivo de configuración que define el autómata debe ser un `.txt` con un orden estrictamente posicional (definido en el `switch` de `ConstruirAutomataDePila`). Las líneas que empiezan por `#` son ignoradas (comentarios).

El orden esperado es:

1. **Estados del autómata (Q):** Separados por espacios (Ej: `q0 q1 q2 q3`)
2. **Alfabeto de la cadena ($\Sigma$):** Símbolos válidos de entrada (Ej: `a b`)
3. **Alfabeto de la pila ($\Gamma$):** Símbolos válidos en la pila (Ej: `a b z`)
4. **Estado inicial ($q_0$):** Un único estado (Ej: `q0`)
5. **Símbolo inicial de la pila ($Z_0$):** Un único símbolo (Ej: `z`)
6. **Estados finales (F):** Separados por espacios (Ej: `q3`)
7. **Transiciones ($\delta$):** A partir de esta línea, cada línea es una transición compuesta por 5 valores separados por espacios:
   * `[Estado_Origen] [Simbolo_Cadena] [Simbolo_Sacar_Pila] [Estado_Destino] [Simbolos_Meter_Pila]`

*(Nota: Se usa `.` para representar épsilon/lambda en el símbolo de la cadena, o cuando no se quiere meter nada en la pila en el quinto valor).*