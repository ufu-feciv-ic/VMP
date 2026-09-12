cmake_minimum_required(VERSION 3.20)
project(VMP CXX)

# Padrao C++ moderno
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Opcoes de compilacao e avisos rigorosos
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Headers publicos disponiveis globalmente
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Reconhecimento automatico de arquivos na pasta src/ e include/
# A flag CONFIGURE_DEPENDS instrui o CMake a reconfigurar automaticamente ao adicionar/remover arquivos
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.hpp")

# Remove src/main.cpp da biblioteca para nao colidir com o ponto de entrada dos testes
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

# Biblioteca estatica contendo as regras de negocio do projeto
add_library(vmp_core STATIC ${LIB_SOURCES} ${HEADERS})
target_include_directories(vmp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Executavel principal (apenas se main.cpp existir)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(vmp_app src/main.cpp)
    target_link_libraries(vmp_app PRIVATE vmp_core)
endif()

# Configuracao da Suite de Testes (TDD)
enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
file(GLOB_RECURSE TEST_HEADERS CONFIGURE_DEPENDS "tests/*.hpp")

add_executable(vmp_tests ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(vmp_tests PRIVATE vmp_core)
target_include_directories(vmp_tests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

add_test(NAME AllTests COMMAND vmp_tests)
