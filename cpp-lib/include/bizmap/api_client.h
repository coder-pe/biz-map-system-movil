// Archivo: api_client.h
// Cliente de API para comunicación con el backend
#pragma once

#include <string>
#include <memory>
#include "models.h"
#include "callbacks.h"

namespace bizmap {

// Forward declarations
namespace internal {
    class HttpClient;
}

/**
 * Cliente de API para comunicación con el backend de BizMap
 *
 * Provee métodos para:
 * - Autenticación (login, registro, perfil)
 * - Búsqueda de negocios
 * - Búsqueda de productos
 * - Historial y recomendaciones
 *
 * Todas las operaciones son asíncronas y usan callbacks.
 */
class ApiClient {
public:
    /**
     * Constructor
     * @param base_url URL base del backend (ej: "http://localhost:8080")
     */
    explicit ApiClient(const std::string& base_url = "http://localhost:8080");

    /**
     * Destructor
     */
    ~ApiClient();

    // No permitir copiar (usa unique_ptr internamente)
    ApiClient(const ApiClient&) = delete;
    ApiClient& operator=(const ApiClient&) = delete;

    // ========== Configuración ==========

    /**
     * Establece la URL base del servidor
     * @param url URL base (ej: "http://192.168.1.100:8080")
     */
    void setBaseUrl(const std::string& url);

    /**
     * Establece el token de autenticación
     * @param token Token JWT obtenido del login
     */
    void setAuthToken(const std::string& token);

    /**
     * Limpia el token de autenticación
     */
    void clearAuthToken();

    /**
     * Obtiene el token actual
     */
    std::string getAuthToken() const;

    // ========== Autenticación ==========

    /**
     * Inicia sesión con credenciales
     * @param username Nombre de usuario
     * @param password Contraseña
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void login(
        const std::string& username,
        const std::string& password,
        LoginSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Registra un nuevo usuario
     * @param request Datos de registro
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void registerUser(
        const RegisterRequest& request,
        VoidSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Cierra la sesión actual
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void logout(
        VoidSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Obtiene el perfil del usuario actual
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void getProfile(
        ProfileSuccessCallback onSuccess,
        ErrorCallback onError
    );

    // ========== Negocios ==========

    /**
     * Busca negocios según parámetros
     * @param params Parámetros de búsqueda
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void searchBusinesses(
        const BusinessSearchParams& params,
        BusinessListSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Obtiene un negocio por ID
     * @param business_id ID del negocio
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void getBusinessById(
        const std::string& business_id,
        BusinessSuccessCallback onSuccess,
        ErrorCallback onError
    );

    // ========== Productos ==========

    /**
     * Busca productos según parámetros
     * @param params Parámetros de búsqueda
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void searchProducts(
        const ProductSearchParams& params,
        ProductListSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Obtiene un producto por ID
     * @param product_id ID del producto
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void getProductById(
        const std::string& product_id,
        ProductSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Obtiene todos los productos de un negocio
     * @param business_id ID del negocio
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void getBusinessProducts(
        const std::string& business_id,
        ProductListSuccessCallback onSuccess,
        ErrorCallback onError
    );

    // ========== Historial y Recomendaciones ==========

    /**
     * Obtiene el historial de búsquedas del usuario
     * @param limit Número máximo de resultados
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void getSearchHistory(
        int limit,
        SearchHistorySuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Obtiene recomendaciones personalizadas
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void getRecommendations(
        RecommendationsSuccessCallback onSuccess,
        ErrorCallback onError
    );

    /**
     * Limpia el historial de búsquedas
     * @param onSuccess Callback en caso de éxito
     * @param onError Callback en caso de error
     */
    void clearSearchHistory(
        VoidSuccessCallback onSuccess,
        ErrorCallback onError
    );

private:
    std::string base_url_;
    std::string auth_token_;
    std::unique_ptr<internal::HttpClient> http_client_;

    // Métodos auxiliares internos
    std::string buildUrl(const std::string& endpoint) const;
};

} // namespace bizmap
