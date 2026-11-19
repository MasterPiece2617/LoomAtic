# LoomAtic

Loomatic es un lenguaje de alto nivel, de paradigma híbrido y con tipado estático y fuerte. 

Aunque está pensado para aprovechar un scripter visual que evite ambigüedades sintácticas, todo el código se almacena en texto plano dentro de archivos .lm escritos en la propia sintaxis de Loomatic. Esto permite que el archivo no solo sea la entrada directa para el compilador, sino que también pueda ser leído y modificado manualmente por el programador. Además, el código admite etiquetas internas que guardan información sobre la disposición y el estilo visual de cada bloque, permitiendo reconstruir el diagrama exacto al abrir el archivo en la interfaz gráfica.

# Compilación:

- Linux:

```bash
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

- Windows: 

```bash
cmd /c "rmdir /s /q build"
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

# Uso del lexer

Los programas fuente de LoomAtic usan la extension `.lm`. Desde la raiz del
proyecto, ejecuta el programa compilado y pasa el archivo que quieres analizar:

```bash
build/main prueba.lm
```

En Windows, el ejecutable es `build/main.exe`.

# Funcionamiento del Analizador Léxico de LoomAtic

El analizador léxico (*lexer*) de **LoomAtic** está diseñado para transformar un flujo de texto plano (archivos fuente `.lm`) en un conjunto estructurado de componentes léxicos (**tokens**), sirviendo como la primera fase crítica del compilador. 

Debido a que el proyecto se apoya en el motor de expresiones regulares de la librería externa `DeSiGNAR`, la arquitectura implementa un **preprocesador híbrido a nivel de código fuente** combinado con un autómata basado en patrones regulares.

El sistema se divide en cuatro subsistemas principales:

## 1. Definición de la Gramática y Reglas Léxicas (`add_language_rules`)

El lexer inicializa su conjunto de reglas estandarizadas categorizando los elementos del lenguaje:

* **Palabras Reservadas (*Keywords*):** Se asignan con categorías explícitas prefijadas por tipo (como `KW_INT`, `KW_IF`, `KW_WHILE`, `KW_RETURN`). Los valores booleanos `true` y `false` se clasifican directamente como `BOOL_LITERAL`.
* **Identificadores y Literales Numéricos:** 
  * Los identificadores (`IDENTIFIER`) siguen el patrón estándar de una letra o `_` inicial seguida de cero o más letras, `_` o dígitos: `[a-zA-Z][a-zA-Z0-9]*`.
  * Los enteros (`INTEGER_LITERAL`) y números reales/flotantes (`REAL_LITERAL`) se configuran mediante cuantificadores de repetición (`+`) para capturar secuencias numéricas de longitud arbitraria y expresiones con punto decimal.
* **Operadores y Delimitadores:** Se registran operadores lógicos (`&&`, `||`), relacionales (`==`, `!=`, `>=`, `<=`), aritméticos (`+`, `-`, `*`, `/`) y signos de puntuación (`{}`, `()`, `[]`, `;`, `,`), excluyendo correctamente los caracteres especiales de las expresiones regulares (ej. `\\+`, `\\(`).

## 2. El Preprocesador de Excepciones Léxicas (`tokenize_source`)

El lexer implementa un **barrido lineal previo** sobre el búfer del archivo fuente antes de delegar en `DeSiGNAR`:

* **Filtrado de Comentarios de Línea (`//`):** Al detectar la secuencia `//`, el algoritmo intercepta el flujo, ignora de manera absoluta todo el contenido subsiguiente hasta encontrar un salto de línea (`\n` o `\r`), y desplaza el puntero de análisis, evitando que los comentarios ensucien el flujo de tokens.
* **Aislamiento de Cadenas y Caracteres (`"..."` y `'...'`):** Cuando encuentra comillas dobles o simples, el preprocesador aísla el literal de manera estricta:
  * Para los caracteres (`CHAR_LITERAL`), valida que no estén vacíos y que contengan estrictamente un solo carácter, lanzando una excepción de error léxico con su posición exacta (*offset*) en caso contrario.
  * Para las cadenas (`STRING_LITERAL`), avanza de forma segura hasta encontrar la comilla de cierre, controlando errores por cadenas sin terminar.

## 3. Segmentación y Traducción de Fragmentos (`tokenize_regular_text`)

Las porciones de código regular que quedan fuera de los literales de texto o comentarios se acumulan en fragmentos y se envían al motor subyacente `lexer.tokenize()` de la librería `DeSiGNAR`. 

Una vez que el motor devuelve los tokens base, esta función se encarga de **recalcular los *offsets* de posición absoluta** sumando el desplazamiento del fragmento respecto al archivo original (`start + token.position`). Después, `Lexer::tokenize` agrega a cada token su línea y columna (ambas desde 1), tratando `LF`, `CRLF` y `CR` como finales de línea. El offset se conserva desde 0 y se mide en bytes.

## 4. Orquestación y Control de Errores (`main.cpp`)

El punto de entrada (`main`) gestiona el ciclo de vida de la ejecución léxica:

1. **Validación de Entrada:** Verifica estrictamente que el archivo proporcionado por consola posea la extensión oficial `.lm` (*LoomAtic*).
2. **Carga Eficiente:** Lee el archivo completo en un búfer de memoria mediante iteradores de flujo (`std::istreambuf_iterator`).
3. **Filtrado y Salida:** Ejecuta el *pipeline* del lexer, descarta los tokens de tipo `WHITESPACE` (espacios, tabulaciones y saltos de línea) e imprime por la salida estándar únicamente la lista limpia de tokens con su categoría, lexema, línea, columna y offset absoluto:

```text
1:1 (offset 0) KW_INT "int"
1:5 (offset 4) IDENTIFIER "edad"
1:10 (offset 9) ASSIGN "="
1:12 (offset 11) INTEGER_LITERAL "2026"
```
