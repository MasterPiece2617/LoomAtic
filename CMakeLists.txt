cmake_minimum_required(VERSION 3.20)
project(LoomAtic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluimos el módulo nativo de CMake para descargar dependencias externas
include(FetchContent)

# Configuramos DeSiGNAR para que se descargue solo desde GitHub
FetchContent_Declare(
    designar
    GIT_REPOSITORY https://github.com/R3mmurd/DeSiGNAR.git
    GIT_TAG        main # O puedes cambiarlo a un commit/tag específico si el profesor lo actualiza y rompe cosas
)

# Apagar los tests y demos del profesor en la propia directiva de FetchContent
set(DESIGNAR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(DESIGNAR_BUILD_DEMOS OFF CACHE BOOL "" FORCE)

# Hacemos que CMake lo descargue y lo ponga disponible
FetchContent_MakeAvailable(designar)

add_executable(main main.cpp)

# Enlazamos la librería descargada
if(TARGET Designar::Designar)
    target_link_libraries(main PRIVATE Designar::Designar)
elseif(TARGET designar)
    target_link_libraries(main PRIVATE designar)
else()
    target_link_libraries(main PRIVATE Designar)
endif()

# Como FetchContent ya gestiona los includes internamente al hacer el add_subdirectory automático,
# nos quitamos el bucle chapucero de GLOB_RECURSE. Con esto ya debería encontrar todo limpio.
target_include_directories(main PRIVATE 
    ${designar_SOURCE_DIR}
    ${designar_SOURCE_DIR}/include
)