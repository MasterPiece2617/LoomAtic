cmake_minimum_required(VERSION 3.20)
project(LoomAtic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Descargar DeSiGNAR automáticamente
FetchContent_Declare(
    designar
    GIT_REPOSITORY https://github.com/R3mmurd/DeSiGNAR.git
    GIT_TAG        main
)

set(DESIGNAR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(DESIGNAR_BUILD_DEMOS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(designar)

# ¡Aquí está el cambio! El ejecutable ahora busca en src/main.cpp
add_executable(main src/main.cpp)

# Enlazar la librería de DeSiGNAR
if(TARGET Designar::Designar)
    target_link_libraries(main PRIVATE Designar::Designar)
elseif(TARGET designar)
    target_link_libraries(main PRIVATE designar)
else()
    target_link_libraries(main PRIVATE Designar)
endif()

# Definir los directorios de inclusión (nuestra carpeta include/ y DeSiGNAR)
target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${designar_SOURCE_DIR}
    ${designar_SOURCE_DIR}/include
)