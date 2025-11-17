// Archivo: bizmap_c_api.h
// Interfaz C para interoperabilidad con JNI (Android) y Swift (iOS)
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

// ========== Tipos Opacos ==========

/**
 * Handle opaco al ApiClient
 * Usar bizmap_create_client() para crear
 * Usar bizmap_destroy_client() para liberar
 */
typedef void* BizMapClientHandle;

// ========== Callbacks C ==========

/**
 * Callback para respuestas de login
 * @param user_data Datos del usuario pasados al llamar la función
 * @param access_token Token de acceso JWT
 * @param refresh_token Token de refresco
 * @param expires_in Tiempo de expiración en segundos
 */
typedef void (*BizMapLoginCallback)(
    void* user_data,
    const char* access_token,
    const char* refresh_token,
    int expires_in
);

/**
 * Callback para errores
 * @param user_data Datos del usuario
 * @param status_code Código de estado HTTP
 * @param error_message Mensaje de error
 */
typedef void (*BizMapErrorCallback)(
    void* user_data,
    int status_code,
    const char* error_message
);

/**
 * Callback para operaciones sin retorno de datos
 * @param user_data Datos del usuario
 */
typedef void (*BizMapVoidCallback)(void* user_data);

/**
 * Callback para lista de productos
 * @param user_data Datos del usuario
 * @param products_json JSON array de productos como string
 */
typedef void (*BizMapProductsCallback)(
    void* user_data,
    const char* products_json
);

/**
 * Callback para lista de negocios
 * @param user_data Datos del usuario
 * @param businesses_json JSON array de negocios como string
 */
typedef void (*BizMapBusinessesCallback)(
    void* user_data,
    const char* businesses_json
);

/**
 * Callback para perfil de usuario
 * @param user_data Datos del usuario
 * @param profile_json JSON object del perfil como string
 */
typedef void (*BizMapProfileCallback)(
    void* user_data,
    const char* profile_json
);

/**
 * Callback para recomendaciones
 * @param user_data Datos del usuario
 * @param recommendations_json JSON object de recomendaciones
 */
typedef void (*BizMapRecommendationsCallback)(
    void* user_data,
    const char* recommendations_json
);

// ========== Gestión del Cliente ==========

/**
 * Crea un nuevo cliente de API
 * @param base_url URL base del servidor (ej: "http://localhost:8080")
 * @return Handle al cliente (NULL en caso de error)
 */
BizMapClientHandle bizmap_create_client(const char* base_url);

/**
 * Destruye un cliente de API
 * @param client Handle al cliente
 */
void bizmap_destroy_client(BizMapClientHandle client);

/**
 * Establece la URL base del servidor
 * @param client Handle al cliente
 * @param base_url Nueva URL base
 */
void bizmap_set_base_url(BizMapClientHandle client, const char* base_url);

/**
 * Establece el token de autenticación
 * @param client Handle al cliente
 * @param token Token JWT
 */
void bizmap_set_auth_token(BizMapClientHandle client, const char* token);

/**
 * Limpia el token de autenticación
 * @param client Handle al cliente
 */
void bizmap_clear_auth_token(BizMapClientHandle client);

// ========== Autenticación ==========

/**
 * Inicia sesión
 * @param client Handle al cliente
 * @param username Nombre de usuario
 * @param password Contraseña
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario a pasar a los callbacks
 */
void bizmap_login(
    BizMapClientHandle client,
    const char* username,
    const char* password,
    BizMapLoginCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Registra un nuevo usuario
 * @param client Handle al cliente
 * @param username Nombre de usuario
 * @param email Email
 * @param password Contraseña
 * @param full_name Nombre completo
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_register(
    BizMapClientHandle client,
    const char* username,
    const char* email,
    const char* password,
    const char* full_name,
    BizMapVoidCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Cierra sesión
 * @param client Handle al cliente
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_logout(
    BizMapClientHandle client,
    BizMapVoidCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Obtiene el perfil del usuario actual
 * @param client Handle al cliente
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_get_profile(
    BizMapClientHandle client,
    BizMapProfileCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

// ========== Búsqueda de Productos ==========

/**
 * Busca productos
 * @param client Handle al cliente
 * @param query Término de búsqueda
 * @param latitude Latitud (0 si no aplica)
 * @param longitude Longitud (0 si no aplica)
 * @param radius_meters Radio en metros (0 para sin límite)
 * @param min_price Precio mínimo (-1 para sin límite)
 * @param max_price Precio máximo (-1 para sin límite)
 * @param category Categoría (NULL para todas)
 * @param limit Límite de resultados
 * @param offset Offset para paginación
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_search_products(
    BizMapClientHandle client,
    const char* query,
    double latitude,
    double longitude,
    int radius_meters,
    double min_price,
    double max_price,
    const char* category,
    int limit,
    int offset,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Obtiene un producto por ID
 * @param client Handle al cliente
 * @param product_id ID del producto
 * @param on_success Callback de éxito (recibe JSON del producto)
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_get_product(
    BizMapClientHandle client,
    const char* product_id,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

// ========== Búsqueda de Negocios ==========

/**
 * Busca negocios
 * @param client Handle al cliente
 * @param query Término de búsqueda
 * @param latitude Latitud (0 si no aplica)
 * @param longitude Longitud (0 si no aplica)
 * @param radius_meters Radio en metros (0 para sin límite)
 * @param category Categoría (NULL para todas)
 * @param limit Límite de resultados
 * @param offset Offset para paginación
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_search_businesses(
    BizMapClientHandle client,
    const char* query,
    double latitude,
    double longitude,
    int radius_meters,
    const char* category,
    int limit,
    int offset,
    BizMapBusinessesCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Obtiene un negocio por ID
 * @param client Handle al cliente
 * @param business_id ID del negocio
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_get_business(
    BizMapClientHandle client,
    const char* business_id,
    BizMapBusinessesCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Obtiene los productos de un negocio
 * @param client Handle al cliente
 * @param business_id ID del negocio
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_get_business_products(
    BizMapClientHandle client,
    const char* business_id,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

// ========== Historial y Recomendaciones ==========

/**
 * Obtiene el historial de búsquedas
 * @param client Handle al cliente
 * @param limit Límite de resultados
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_get_search_history(
    BizMapClientHandle client,
    int limit,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Obtiene recomendaciones personalizadas
 * @param client Handle al cliente
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_get_recommendations(
    BizMapClientHandle client,
    BizMapRecommendationsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

/**
 * Limpia el historial de búsquedas
 * @param client Handle al cliente
 * @param on_success Callback de éxito
 * @param on_error Callback de error
 * @param user_data Datos del usuario
 */
void bizmap_clear_search_history(
    BizMapClientHandle client,
    BizMapVoidCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
);

// ========== Utilidades ==========

/**
 * Obtiene la versión de la biblioteca
 * @return String con la versión (no liberar)
 */
const char* bizmap_get_version(void);

#ifdef __cplusplus
}
#endif
