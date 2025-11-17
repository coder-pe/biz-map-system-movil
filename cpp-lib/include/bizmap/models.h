// Archivo: models.h
// Modelos de datos para la biblioteca BizMap
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <ctime>

namespace bizmap {

// ========== Estructuras de Ubicación ==========

struct GeoLocation {
    double latitude;
    double longitude;

    GeoLocation() : latitude(0.0), longitude(0.0) {}
    GeoLocation(double lat, double lng) : latitude(lat), longitude(lng) {}

    bool isValid() const {
        return latitude >= -90.0 && latitude <= 90.0 &&
               longitude >= -180.0 && longitude <= 180.0;
    }
};

// ========== Usuario ==========

struct User {
    std::string id;
    std::string username;
    std::string email;
    std::string full_name;
    std::string phone;
    bool is_active;
    time_t created_at;
    time_t updated_at;
};

struct LoginRequest {
    std::string username;
    std::string password;
};

struct RegisterRequest {
    std::string username;
    std::string email;
    std::string password;
    std::string full_name;
    std::string phone;
};

struct AuthResponse {
    std::string access_token;
    std::string refresh_token;
    std::string token_type;
    int expires_in;
};

// ========== Negocio ==========

struct Business {
    std::string id;
    std::string owner_id;
    std::string name;
    std::string description;
    std::string category;
    std::string phone;
    std::string email;
    std::string website;
    std::string address;
    GeoLocation location;
    double rating;
    int total_reviews;
    bool is_verified;
    bool is_active;
    std::vector<std::string> images;
    time_t created_at;
    time_t updated_at;

    // Campo calculado en búsquedas
    std::optional<double> distance_meters;
};

struct BusinessSearchParams {
    std::string query;
    std::optional<std::string> category;
    std::optional<GeoLocation> location;
    std::optional<int> radius_meters;
    int limit = 20;
    int offset = 0;
};

// ========== Producto ==========

struct Product {
    std::string id;
    std::string business_id;
    std::string name;
    std::string description;
    double price;
    std::string currency;
    std::string category;
    bool is_available;
    int stock_quantity;
    std::vector<std::string> images;
    time_t created_at;
    time_t updated_at;
};

struct ProductWithBusiness {
    Product product;
    Business business;
    std::optional<double> distance_meters;
};

struct ProductSearchParams {
    std::string query;
    std::optional<std::string> category;
    std::optional<GeoLocation> location;
    std::optional<int> radius_meters;
    std::optional<double> min_price;
    std::optional<double> max_price;
    int limit = 20;
    int offset = 0;
};

// ========== Historial y Recomendaciones ==========

struct SearchHistoryEntry {
    std::string id;
    std::string user_id;
    std::string query;
    std::string category;
    GeoLocation location;
    time_t created_at;
};

struct UserRecommendations {
    std::vector<std::string> popular_searches;
    std::vector<std::string> popular_categories;
};

// ========== Respuestas de API ==========

struct ApiError {
    int status_code;
    std::string error_message;
    std::string error_code;

    ApiError() : status_code(0) {}
    ApiError(int code, const std::string& message)
        : status_code(code), error_message(message) {}
};

} // namespace bizmap
