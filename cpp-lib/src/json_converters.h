// Archivo: json_converters.h (interno)
// Conversores entre estructuras C++ y JSON
#pragma once

#include "../include/bizmap/models.h"
// Nota: nlohmann/json se incluirá cuando esté disponible
// #include <nlohmann/json.hpp>

// Para ahora, voy a crear funciones helper que asumen que nlohmann/json está incluido

namespace bizmap {
namespace json {

// Forward declaration para evitar incluir JSON aquí
// El tipo real será nlohmann::json
class json_type;

/**
 * Conversores de C++ a JSON
 */
namespace to_json {
    std::string login_request(const std::string& username, const std::string& password);
    std::string register_request(const RegisterRequest& req);
    std::string business_search_params(const BusinessSearchParams& params);
    std::string product_search_params(const ProductSearchParams& params);
}

/**
 * Conversores de JSON a C++
 */
namespace from_json {
    AuthResponse parse_auth_response(const std::string& json_str);
    User parse_user(const std::string& json_str);
    Business parse_business(const std::string& json_str);
    Product parse_product(const std::string& json_str);
    ProductWithBusiness parse_product_with_business(const std::string& json_str);
    std::vector<Business> parse_business_list(const std::string& json_str);
    std::vector<ProductWithBusiness> parse_product_list(const std::string& json_str);
    UserRecommendations parse_recommendations(const std::string& json_str);
    std::vector<SearchHistoryEntry> parse_search_history(const std::string& json_str);
    ApiError parse_error(int status_code, const std::string& json_str);
}

} // namespace json
} // namespace bizmap
