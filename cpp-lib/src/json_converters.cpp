// Archivo: json_converters.cpp
#include "json_converters.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// NOTA: Este archivo usa strings manualmente para compatibilidad inicial
// En producción, descomentar y usar nlohmann/json:
// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

namespace bizmap {
namespace json {

// ========== Helpers internos ==========

namespace {
    std::string escape_json_string(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }

    std::string optional_string_field(const std::string& name, const std::optional<std::string>& value) {
        if (value.has_value()) {
            return "\"" + name + "\":\"" + escape_json_string(value.value()) + "\"";
        }
        return "";
    }

    std::string optional_double_field(const std::string& name, const std::optional<double>& value) {
        if (value.has_value()) {
            return "\"" + name + "\":" + std::to_string(value.value());
        }
        return "";
    }

    std::string optional_int_field(const std::string& name, const std::optional<int>& value) {
        if (value.has_value()) {
            return "\"" + name + "\":" + std::to_string(value.value());
        }
        return "";
    }
}

// ========== To JSON ==========

namespace to_json {

std::string login_request(const std::string& username, const std::string& password) {
    // Con nlohmann/json sería:
    // json j = {{"username", username}, {"password", password}};
    // return j.dump();

    std::ostringstream oss;
    oss << "{"
        << "\"username\":\"" << escape_json_string(username) << "\","
        << "\"password\":\"" << escape_json_string(password) << "\""
        << "}";
    return oss.str();
}

std::string register_request(const RegisterRequest& req) {
    std::ostringstream oss;
    oss << "{"
        << "\"username\":\"" << escape_json_string(req.username) << "\","
        << "\"email\":\"" << escape_json_string(req.email) << "\","
        << "\"password\":\"" << escape_json_string(req.password) << "\","
        << "\"full_name\":\"" << escape_json_string(req.full_name) << "\"";

    if (!req.phone.empty()) {
        oss << ",\"phone\":\"" << escape_json_string(req.phone) << "\"";
    }

    oss << "}";
    return oss.str();
}

std::string business_search_params(const BusinessSearchParams& params) {
    // Esto se convertiría en query params, no JSON body
    // Retornamos string vacío ya que se construye en ApiClient
    return "";
}

std::string product_search_params(const ProductSearchParams& params) {
    // Esto se convertiría en query params, no JSON body
    return "";
}

} // namespace to_json

// ========== From JSON ==========

namespace from_json {

// NOTA: Estas implementaciones son simplificadas
// En producción usar nlohmann/json para parsing robusto

AuthResponse parse_auth_response(const std::string& json_str) {
    AuthResponse response;

    // Implementación simplificada - en producción usar nlohmann/json
    // json j = json::parse(json_str);
    // response.access_token = j["access_token"].get<std::string>();
    // response.refresh_token = j["refresh_token"].get<std::string>();
    // response.token_type = j["token_type"].get<std::string>();
    // response.expires_in = j["expires_in"].get<int>();

    // Por ahora retornar vacío - se implementará con nlohmann/json
    response.access_token = "";
    response.refresh_token = "";
    response.token_type = "Bearer";
    response.expires_in = 86400;

    return response;
}

User parse_user(const std::string& json_str) {
    User user;
    // Implementar con nlohmann/json
    return user;
}

Business parse_business(const std::string& json_str) {
    Business business;
    // Implementar con nlohmann/json
    return business;
}

Product parse_product(const std::string& json_str) {
    Product product;
    // Implementar con nlohmann/json
    return product;
}

ProductWithBusiness parse_product_with_business(const std::string& json_str) {
    ProductWithBusiness pwb;
    // Implementar con nlohmann/json
    return pwb;
}

std::vector<Business> parse_business_list(const std::string& json_str) {
    std::vector<Business> businesses;
    // Implementar con nlohmann/json
    return businesses;
}

std::vector<ProductWithBusiness> parse_product_list(const std::string& json_str) {
    std::vector<ProductWithBusiness> products;
    // Implementar con nlohmann/json
    return products;
}

UserRecommendations parse_recommendations(const std::string& json_str) {
    UserRecommendations recs;
    // Implementar con nlohmann/json
    return recs;
}

std::vector<SearchHistoryEntry> parse_search_history(const std::string& json_str) {
    std::vector<SearchHistoryEntry> history;
    // Implementar con nlohmann/json
    return history;
}

ApiError parse_error(int status_code, const std::string& json_str) {
    ApiError error;
    error.status_code = status_code;

    // Intentar parsear mensaje de error del JSON
    // json j = json::parse(json_str);
    // if (j.contains("error")) {
    //     error.error_message = j["error"].get<std::string>();
    // }

    error.error_message = json_str; // Por ahora usar el JSON completo

    return error;
}

} // namespace from_json

} // namespace json
} // namespace bizmap
