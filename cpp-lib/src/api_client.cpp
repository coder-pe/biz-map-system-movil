// Archivo: api_client.cpp
#include "../include/bizmap/api_client.h"
#include "http_client.h"
#include "json_converters.h"
#include <sstream>
#include <iostream>

namespace bizmap {

// ========== Constructor / Destructor ==========

ApiClient::ApiClient(const std::string& base_url)
    : base_url_(base_url)
    , auth_token_()
    , http_client_(std::make_unique<internal::HttpClient>(base_url))
{
}

ApiClient::~ApiClient() {
}

// ========== Configuración ==========

void ApiClient::setBaseUrl(const std::string& url) {
    base_url_ = url;
    // Recrear cliente HTTP con nueva URL
    http_client_ = std::make_unique<internal::HttpClient>(url);
}

void ApiClient::setAuthToken(const std::string& token) {
    auth_token_ = token;
}

void ApiClient::clearAuthToken() {
    auth_token_.clear();
}

std::string ApiClient::getAuthToken() const {
    return auth_token_;
}

std::string ApiClient::buildUrl(const std::string& endpoint) const {
    return endpoint;
}

// ========== Autenticación ==========

void ApiClient::login(
    const std::string& username,
    const std::string& password,
    LoginSuccessCallback onSuccess,
    ErrorCallback onError
) {
    std::string body = json::to_json::login_request(username, password);
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    http_client_->post(
        "/api/v1/auth/login",
        body,
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                AuthResponse auth = json::from_json::parse_auth_response(response_body);
                onSuccess(auth);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::registerUser(
    const RegisterRequest& request,
    VoidSuccessCallback onSuccess,
    ErrorCallback onError
) {
    std::string body = json::to_json::register_request(request);
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    http_client_->post(
        "/api/v1/auth/register",
        body,
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                onSuccess();
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::logout(
    VoidSuccessCallback onSuccess,
    ErrorCallback onError
) {
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + auth_token_}
    };

    http_client_->post(
        "/api/v1/auth/logout",
        "{}",
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                onSuccess();
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::getProfile(
    ProfileSuccessCallback onSuccess,
    ErrorCallback onError
) {
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + auth_token_}
    };

    http_client_->get(
        "/api/v1/auth/profile",
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                User user = json::from_json::parse_user(response_body);
                onSuccess(user);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

// ========== Negocios ==========

void ApiClient::searchBusinesses(
    const BusinessSearchParams& params,
    BusinessListSuccessCallback onSuccess,
    ErrorCallback onError
) {
    // Construir query params
    std::ostringstream oss;
    oss << "/api/v1/businesses/search?q=" << params.query;

    if (params.location.has_value()) {
        oss << "&lat=" << params.location->latitude
            << "&lng=" << params.location->longitude;

        if (params.radius_meters.has_value()) {
            oss << "&radius=" << params.radius_meters.value();
        }
    }

    if (params.category.has_value()) {
        oss << "&category=" << params.category.value();
    }

    oss << "&limit=" << params.limit
        << "&offset=" << params.offset;

    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    http_client_->get(
        oss.str(),
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                std::vector<Business> businesses = json::from_json::parse_business_list(response_body);
                onSuccess(businesses);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::getBusinessById(
    const std::string& business_id,
    BusinessSuccessCallback onSuccess,
    ErrorCallback onError
) {
    std::string endpoint = "/api/v1/businesses/" + business_id;
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    http_client_->get(
        endpoint,
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                Business business = json::from_json::parse_business(response_body);
                onSuccess(business);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

// ========== Productos ==========

void ApiClient::searchProducts(
    const ProductSearchParams& params,
    ProductListSuccessCallback onSuccess,
    ErrorCallback onError
) {
    // Construir query params
    std::ostringstream oss;
    oss << "/api/v1/products/search?q=" << params.query;

    if (params.location.has_value()) {
        oss << "&lat=" << params.location->latitude
            << "&lng=" << params.location->longitude;

        if (params.radius_meters.has_value()) {
            oss << "&radius=" << params.radius_meters.value();
        }
    }

    if (params.min_price.has_value()) {
        oss << "&min_price=" << params.min_price.value();
    }

    if (params.max_price.has_value()) {
        oss << "&max_price=" << params.max_price.value();
    }

    if (params.category.has_value()) {
        oss << "&category=" << params.category.value();
    }

    oss << "&limit=" << params.limit
        << "&offset=" << params.offset;

    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    // Agregar auth si está disponible
    if (!auth_token_.empty()) {
        headers["Authorization"] = "Bearer " + auth_token_;
    }

    http_client_->get(
        oss.str(),
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                std::vector<ProductWithBusiness> products = json::from_json::parse_product_list(response_body);
                onSuccess(products);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::getProductById(
    const std::string& product_id,
    ProductSuccessCallback onSuccess,
    ErrorCallback onError
) {
    std::string endpoint = "/api/v1/products/" + product_id;
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    http_client_->get(
        endpoint,
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                Product product = json::from_json::parse_product(response_body);
                onSuccess(product);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::getBusinessProducts(
    const std::string& business_id,
    ProductListSuccessCallback onSuccess,
    ErrorCallback onError
) {
    std::string endpoint = "/api/v1/products/business/" + business_id;
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"}
    };

    http_client_->get(
        endpoint,
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                std::vector<ProductWithBusiness> products = json::from_json::parse_product_list(response_body);
                onSuccess(products);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

// ========== Historial y Recomendaciones ==========

void ApiClient::getSearchHistory(
    int limit,
    SearchHistorySuccessCallback onSuccess,
    ErrorCallback onError
) {
    std::string endpoint = "/api/v1/search/history?limit=" + std::to_string(limit);
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + auth_token_}
    };

    http_client_->get(
        endpoint,
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                std::vector<SearchHistoryEntry> history = json::from_json::parse_search_history(response_body);
                onSuccess(history);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::getRecommendations(
    RecommendationsSuccessCallback onSuccess,
    ErrorCallback onError
) {
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + auth_token_}
    };

    http_client_->get(
        "/api/v1/search/recommendations",
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                UserRecommendations recs = json::from_json::parse_recommendations(response_body);
                onSuccess(recs);
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

void ApiClient::clearSearchHistory(
    VoidSuccessCallback onSuccess,
    ErrorCallback onError
) {
    internal::HttpClient::Headers headers = {
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + auth_token_}
    };

    http_client_->del(
        "/api/v1/search/history",
        headers,
        [onSuccess, onError](int status, const std::string& response_body) {
            if (status >= 200 && status < 300) {
                onSuccess();
            } else {
                ApiError error = json::from_json::parse_error(status, response_body);
                onError(error);
            }
        },
        [onError](const std::string& error_msg) {
            ApiError error(0, error_msg);
            onError(error);
        }
    );
}

} // namespace bizmap
