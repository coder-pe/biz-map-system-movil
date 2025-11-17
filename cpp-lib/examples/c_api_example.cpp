// c_api_example.cpp
// Ejemplo de uso de la C API de BizMap (para JNI/Swift)

#include <bizmap/bizmap_c_api.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

// Variables globales para callbacks
bool g_login_done = false;
bool g_login_success = false;
char g_access_token[512] = {0};

// Callback de login
void on_login_success(void* user_data, const char* access_token, const char* refresh_token, int expires_in) {
    std::cout << "✓ Login exitoso!" << std::endl;
    std::cout << "  Token: " << std::string(access_token).substr(0, 20) << "..." << std::endl;
    std::cout << "  Expira en: " << expires_in << " segundos" << std::endl;

    strncpy(g_access_token, access_token, sizeof(g_access_token) - 1);
    g_login_success = true;
    g_login_done = true;
}

void on_login_error(void* user_data, int status_code, const char* error_message) {
    std::cout << "✗ Error en login (" << status_code << "): " << error_message << std::endl;
    g_login_done = true;
}

// Callback de búsqueda
void on_search_success(void* user_data, const char* json_results) {
    std::cout << "✓ Búsqueda exitosa!" << std::endl;
    std::cout << "  Resultados JSON: " << std::string(json_results).substr(0, 100) << "..." << std::endl;
}

void on_search_error(void* user_data, int status_code, const char* error_message) {
    std::cout << "✗ Error en búsqueda (" << status_code << "): " << error_message << std::endl;
}

// Callback genérico
void on_void_success(void* user_data) {
    std::cout << "✓ Operación completada exitosamente" << std::endl;
}

void on_error(void* user_data, int status_code, const char* error_message) {
    std::cout << "✗ Error (" << status_code << "): " << error_message << std::endl;
}

int main() {
    std::cout << "=== BizMap C API - Ejemplo ===" << std::endl;
    std::cout << "(Esta API es útil para JNI/Swift integration)" << std::endl;

    // 1. Crear cliente
    std::cout << "\n1. Creando cliente..." << std::endl;
    BizMapClientHandle client = bizmap_create_client("http://localhost:8080");

    if (!client) {
        std::cout << "✗ Error creando cliente" << std::endl;
        return 1;
    }
    std::cout << "✓ Cliente creado" << std::endl;

    // 2. Login
    std::cout << "\n2. Iniciando sesión..." << std::endl;
    bizmap_login(
        client,
        "testuser",
        "testpass123",
        on_login_success,
        on_login_error,
        nullptr  // user_data
    );

    // Esperar resultado
    while (!g_login_done) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!g_login_success) {
        std::cout << "\nNo se pudo iniciar sesión. Terminando." << std::endl;
        bizmap_destroy_client(client);
        return 1;
    }

    // Configurar token
    bizmap_set_auth_token(client, g_access_token);

    // 3. Buscar negocios
    std::cout << "\n3. Buscando negocios..." << std::endl;
    bizmap_search_businesses(
        client,
        "pizza",          // query
        40.7128,          // latitude
        -74.0060,         // longitude
        5000,             // radius_meters
        nullptr,          // category
        5,                // limit
        0,                // offset
        on_search_success,
        on_search_error,
        nullptr           // user_data
    );

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 4. Buscar productos
    std::cout << "\n4. Buscando productos..." << std::endl;
    bizmap_search_products(
        client,
        "laptop",         // query
        40.7128,          // latitude
        -74.0060,         // longitude
        10000,            // radius_meters
        0.0,              // min_price
        0.0,              // max_price
        nullptr,          // category
        10,               // limit
        0,                // offset
        on_search_success,
        on_search_error,
        nullptr           // user_data
    );

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 5. Logout
    std::cout << "\n5. Cerrando sesión..." << std::endl;
    bizmap_logout(
        client,
        on_void_success,
        on_error,
        nullptr
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 6. Destruir cliente
    std::cout << "\n6. Liberando recursos..." << std::endl;
    bizmap_destroy_client(client);
    std::cout << "✓ Cliente destruido" << std::endl;

    std::cout << "\n=== Ejemplo C API completado ===" << std::endl;
    std::cout << "\nNota: Esta API C puede ser fácilmente integrada con:" << std::endl;
    std::cout << "  - JNI (Java Native Interface) para Android/Kotlin" << std::endl;
    std::cout << "  - Swift bridging headers para iOS" << std::endl;

    return 0;
}
