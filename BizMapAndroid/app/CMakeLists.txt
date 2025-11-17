cmake_minimum_required(VERSION 3.22.1)
project(bizmap_android)

# Configurar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ruta a la biblioteca C++
set(BIZMAP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../cpp-lib")

# Incluir headers de BizMap
include_directories(
    ${BIZMAP_ROOT}/include
    ${BIZMAP_ROOT}/third_party
)

# Archivos fuente de la biblioteca C++
file(GLOB BIZMAP_SOURCES
    "${BIZMAP_ROOT}/src/*.cpp"
)

# Archivos JNI
set(JNI_SOURCES
    src/main/cpp/jni_bridge.cpp
)

# Crear biblioteca compartida
add_library(bizmap SHARED
    ${BIZMAP_SOURCES}
    ${JNI_SOURCES}
)

# Enlazar con bibliotecas de Android
find_library(log-lib log)
target_link_libraries(bizmap ${log-lib})
