// Archivo: bizmap.h
// Header principal de la biblioteca BizMap
// Incluye todos los componentes necesarios
#pragma once

// Versión de la biblioteca
#define BIZMAP_VERSION_MAJOR 1
#define BIZMAP_VERSION_MINOR 0
#define BIZMAP_VERSION_PATCH 0

// Incluir todos los headers públicos
#include "models.h"
#include "callbacks.h"
#include "api_client.h"

/**
 * Biblioteca BizMap - Sistema de localización de productos y servicios
 *
 * Esta biblioteca provee funcionalidad para:
 * - Autenticación de usuarios
 * - Búsqueda de negocios por ubicación
 * - Búsqueda de productos con filtros avanzados
 * - Historial de búsquedas y recomendaciones
 *
 * Diseñada para integrarse con aplicaciones móviles en:
 * - Android (Kotlin/Java vía JNI)
 * - iOS (Swift vía C bridging)
 *
 * Ejemplo de uso básico:
 *
 * ```cpp
 * #include <bizmap/bizmap.h>
 *
 * using namespace bizmap;
 *
 * // Crear cliente
 * ApiClient client("http://192.168.1.100:8080");
 *
 * // Login
 * client.login("usuario", "password",
 *     [](const AuthResponse& response) {
 *         std::cout << "Token: " << response.access_token << std::endl;
 *     },
 *     [](const ApiError& error) {
 *         std::cerr << "Error: " << error.error_message << std::endl;
 *     }
 * );
 *
 * // Buscar productos
 * ProductSearchParams params;
 * params.query = "laptop";
 * params.location = GeoLocation(-12.0464, -77.0428); // Lima, Perú
 * params.radius_meters = 5000; // 5 km
 * params.min_price = 500.0;
 * params.max_price = 2000.0;
 *
 * client.searchProducts(params,
 *     [](const std::vector<ProductWithBusiness>& products) {
 *         for (const auto& p : products) {
 *             std::cout << p.product.name << " - $" << p.product.price << std::endl;
 *         }
 *     },
 *     [](const ApiError& error) {
 *         std::cerr << "Error: " << error.error_message << std::endl;
 *     }
 * );
 * ```
 */

namespace bizmap {

/**
 * Obtiene la versión de la biblioteca como string
 * @return Versión en formato "X.Y.Z"
 */
inline std::string getVersion() {
    return std::to_string(BIZMAP_VERSION_MAJOR) + "." +
           std::to_string(BIZMAP_VERSION_MINOR) + "." +
           std::to_string(BIZMAP_VERSION_PATCH);
}

} // namespace bizmap
