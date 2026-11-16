# LoomAtic
Loom de telar, atic de automatic

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

El lexer usa `Designar::Lexer` para palabras reservadas, identificadores,
numeros y operadores. Aísla las cadenas, caracteres y comentarios `//` porque
la sintaxis regex de DeSiGNAR no incluye clases de caracteres. Muestra cada
token con su lexema y su posicion inicial (offset desde 0); omite espacios y
comentarios de la salida.