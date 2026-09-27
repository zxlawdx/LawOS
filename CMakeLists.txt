# Versão mínima do CMake
cmake_minimum_required(VERSION 3.16)

# Nome do projeto e linguagem
project(LawOS LANGUAGES C)

# Utilizar o padrão C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Criar o executável
add_executable(lawos
    src/main.c
    src/processos.c
    src/completador.c
    src/lawshell.c
)

# Vincular a biblioteca GNU Readline
target_link_libraries(lawos PRIVATE
    readline
)

# Localização dos arquivos .h
target_include_directories(lawos PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Habilitar avisos do compilador
target_compile_options(lawos PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)