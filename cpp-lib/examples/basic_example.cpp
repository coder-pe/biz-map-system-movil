// basic_example.cpp
// Ejemplo básico de uso de la API de BizMap (C++)

#include <bizmap/bizmap.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace bizmap;

int main() {
    std::cout << "=== BizMap C++ API - Ejemplo Básico ===" << std::endl;

    // Crear cliente API
    ApiClient client("http://localhost:8080");

    // Variable para controlar cuando terminar
    bool loginComplete = false;
    bool loginSuccess = false;

    // 1. Login
    std::cout << "\n1. Iniciando sesión..." << std::endl;
    client.login(
        "testuser",
        "testpass123",
        [&](const AuthResponse& auth) {
            std::cout << "   ✓ Login exitoso!" << std::endl;
            std::cout << "   Token: " << auth.access_token.substr(0, 20) << "..." << std::endl;
            std::cout << "   Expira en: " << auth.expires_in << " segundos" << std::endl;

            // Guardar token para futuras peticiones
            client.setAuthToken(auth.access_token);

            loginSuccess = true;
            loginComplete = true;
        },
        [&](const ApiError& error) {
            std::cout << "   ✗ Error en login: " << error.error_message << std::endl;
            loginComplete = true;
        }
    );

    // Esperar a que termine el login
    while (!loginComplete) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!loginSuccess) {
        std::cout << "\nNo se pudo iniciar sesión. Terminando." << std::endl;
        return 1;
    }

    // 2. Buscar negocios
    std::cout << "\n2. Buscando negocios..." << std::endl;
    bool searchComplete = false;

    BusinessSearchParams params;
    params.query = "pizza";
    params.location = GeoLocation(40.7128, -74.0060); // Nueva York
    params.radius_meters = 5000;
    params.limit = 5;

    client.searchBusinesses(
        params,
        [&](const std::vector<Business>& businesses) {
            std::cout << "   ✓ Encontrados " << businesses.size() << " negocios:" << std::endl;
            for (const auto& biz : businesses) {
                std::cout << "   - " << biz.name << " (" << biz.category << ")" << std::endl;
                std::cout << "     Dirección: " << biz.address << std::endl;
                if (biz.rating > 0.0) {
                    std::cout << "     Rating: " << biz.rating << "/5.0" << std::endl;
                }
            }
            searchComplete = true;
        },
        [&](const ApiError& error) {
            std::cout << "   ✗ Error en búsqueda: " << error.error_message << std::endl;
            searchComplete = true;
        }
    );

    while (!searchComplete) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 3. Obtener perfil
    std::cout << "\n3. Obteniendo perfil de usuario..." << std::endl;
    bool profileComplete = false;

    client.getProfile(
        [&](const User& user) {
            std::cout << "   ✓ Perfil obtenido:" << std::endl;
            std::cout << "   - ID: " << user.id << std::endl;
            std::cout << "   - Username: " << user.username << std::endl;
            std::cout << "   - Email: " << user.email << std::endl;
            std::cout << "   - Nombre completo: " << user.full_name << std::endl;
            profileComplete = true;
        },
        [&](const ApiError& error) {
            std::cout << "   ✗ Error obteniendo perfil: " << error.error_message << std::endl;
            profileComplete = true;
        }
    );

    while (!profileComplete) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 4. Logout
    std::cout << "\n4. Cerrando sesión..." << std::endl;
    bool logoutComplete = false;

    client.logout(
        [&]() {
            std::cout << "   ✓ Sesión cerrada correctamente" << std::endl;
            client.clearAuthToken();
            logoutComplete = true;
        },
        [&](const ApiError& error) {
            std::cout << "   ✗ Error en logout: " << error.error_message << std::endl;
            logoutComplete = true;
        }
    );

    while (!logoutComplete) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\n=== Ejemplo completado ===" << std::endl;
    return 0;
}
