cmake_minimum_required(VERSION 3.10)
project(SistemaSolar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ativa mensagens mais claras em versões recentes do CMake
cmake_policy(SET CMP0072 NEW)

# Busca dependências
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Adiciona o executável principal
add_executable(SistemaSolar main.cpp)

# Linka bibliotecas necessárias
target_link_libraries(SistemaSolar
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    GLU
    GL
)

# Inclui diretórios (útil se futuramente separar headers)
target_include_directories(SistemaSolar PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
