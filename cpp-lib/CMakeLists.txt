# CMakeLists.txt para BizMap C++ Library
cmake_minimum_required(VERSION 3.14)
project(BizMapLib VERSION 1.0.0 LANGUAGES CXX)

# Configuración del estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación
option(BIZMAP_BUILD_SHARED "Build shared library" ON)
option(BIZMAP_BUILD_EXAMPLES "Build example programs" ON)
option(BIZMAP_ENABLE_SSL "Enable HTTPS support (requires OpenSSL)" OFF)

# Detectar plataforma
if(ANDROID)
    message(STATUS "Building for Android")
    set(BIZMAP_PLATFORM "android")
elseif(IOS)
    message(STATUS "Building for iOS")
    set(BIZMAP_PLATFORM "ios")
else()
    message(STATUS "Building for Desktop")
    set(BIZMAP_PLATFORM "desktop")
endif()

# ========== Dependencias ==========

# Intentar encontrar nlohmann/json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found, using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        json
	URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

# cpp-httplib (header-only)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/httplib.h")
    message(WARNING "cpp-httplib not found in third_party/")
    message(STATUS "Downloading cpp-httplib...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/httplib.h"
        SHOW_PROGRESS
    )
endif()

# OpenSSL (opcional, para HTTPS)
if(BIZMAP_ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
    add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT)
endif()

# ========== Biblioteca Principal ==========

# Archivos fuente
set(BIZMAP_SOURCES
    src/api_client.cpp
    src/http_client.cpp
    src/json_converters.cpp
    src/bizmap_c_api.cpp
)

# Headers públicos
set(BIZMAP_PUBLIC_HEADERS
    include/bizmap/models.h
    include/bizmap/callbacks.h
    include/bizmap/api_client.h
    include/bizmap/bizmap.h
    include/bizmap/bizmap_c_api.h
)

# Crear biblioteca
if(BIZMAP_BUILD_SHARED)
    add_library(bizmap SHARED ${BIZMAP_SOURCES})
else()
    add_library(bizmap STATIC ${BIZMAP_SOURCES})
endif()

# Alias para uso con target_link_libraries
add_library(BizMap::bizmap ALIAS bizmap)

# Directorios de include
target_include_directories(bizmap
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Enlazar con dependencias
target_link_libraries(bizmap
    PUBLIC
        nlohmann_json::nlohmann_json
)

if(BIZMAP_ENABLE_SSL)
    target_link_libraries(bizmap PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# Configuración específica de plataforma
if(ANDROID)
    # Para Android, agregar soporte de log
    target_link_libraries(bizmap PRIVATE log)
elseif(IOS)
    # Para iOS, configuraciones específicas
    set_target_properties(bizmap PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A
        MACOSX_FRAMEWORK_IDENTIFIER com.aisoldev.bizmap
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER "${BIZMAP_PUBLIC_HEADERS}"
    )
elseif(UNIX AND NOT APPLE)
    # Linux
    target_link_libraries(bizmap PRIVATE pthread)
endif()

# Propiedades de compilación
target_compile_options(bizmap PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ========== Ejemplos ==========

if(BIZMAP_BUILD_EXAMPLES AND NOT ANDROID AND NOT IOS)
    add_subdirectory(examples)
endif()

# ========== Instalación ==========

include(GNUInstallDirs)

# Instalar biblioteca
install(TARGETS bizmap
    EXPORT BizMapTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bizmap
)

# Instalar headers
install(DIRECTORY include/bizmap
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Exportar targets
install(EXPORT BizMapTargets
    FILE BizMapTargets.cmake
    NAMESPACE BizMap::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BizMap
)

# Crear config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/BizMapConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BizMapConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/BizMapConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BizMap
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/BizMapConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/BizMapConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BizMap
)

# ========== Información de compilación ==========

message(STATUS "")
message(STATUS "BizMap Library Configuration:")
message(STATUS "  Version:       ${PROJECT_VERSION}")
message(STATUS "  Platform:      ${BIZMAP_PLATFORM}")
message(STATUS "  C++ Standard:  ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  Shared Lib:    ${BIZMAP_BUILD_SHARED}")
message(STATUS "  SSL Support:   ${BIZMAP_ENABLE_SSL}")
message(STATUS "  Build Examples:${BIZMAP_BUILD_EXAMPLES}")
message(STATUS "")
