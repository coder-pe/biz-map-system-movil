// Archivo: bizmap_c_api.cpp
// Implementación de la interfaz C para JNI y Swift
#include "../include/bizmap/bizmap_c_api.h"
#include "../include/bizmap/bizmap.h"
#include <string>
#include <memory>
#include <cstring>

using namespace bizmap;

// ========== Estructuras internas para callbacks ==========

struct CallbackContext {
    void* user_data;
    BizMapLoginCallback login_callback;
    BizMapErrorCallback error_callback;
    BizMapVoidCallback void_callback;
    BizMapProductsCallback products_callback;
    BizMapBusinessesCallback businesses_callback;
    BizMapProfileCallback profile_callback;
    BizMapRecommendationsCallback recommendations_callback;
};

// ========== Helper para convertir string C++ a C ==========

static char* create_c_string(const std::string& str) {
    char* c_str = new char[str.length() + 1];
    std::strcpy(c_str, str.c_str());
    return c_str;
}

// ========== Gestión del Cliente ==========

BizMapClientHandle bizmap_create_client(const char* base_url) {
    try {
        std::string url = base_url ? base_url : "http://localhost:8080";
        auto* client = new ApiClient(url);
        return static_cast<void*>(client);
    } catch (...) {
        return nullptr;
    }
}

void bizmap_destroy_client(BizMapClientHandle client) {
    if (client) {
        delete static_cast<ApiClient*>(client);
    }
}

void bizmap_set_base_url(BizMapClientHandle client, const char* base_url) {
    if (client && base_url) {
        static_cast<ApiClient*>(client)->setBaseUrl(base_url);
    }
}

void bizmap_set_auth_token(BizMapClientHandle client, const char* token) {
    if (client && token) {
        static_cast<ApiClient*>(client)->setAuthToken(token);
    }
}

void bizmap_clear_auth_token(BizMapClientHandle client) {
    if (client) {
        static_cast<ApiClient*>(client)->clearAuthToken();
    }
}

// ========== Autenticación ==========

void bizmap_login(
    BizMapClientHandle client,
    const char* username,
    const char* password,
    BizMapLoginCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client || !username || !password) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data, on_success, on_error};

    api->login(
        username,
        password,
        [ctx](const AuthResponse& response) {
            if (ctx->login_callback) {
                ctx->login_callback(
                    ctx->user_data,
                    response.access_token.c_str(),
                    response.refresh_token.c_str(),
                    response.expires_in
                );
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_register(
    BizMapClientHandle client,
    const char* username,
    const char* email,
    const char* password,
    const char* full_name,
    BizMapVoidCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client || !username || !email || !password) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->void_callback = on_success;
    ctx->error_callback = on_error;

    RegisterRequest req;
    req.username = username;
    req.email = email;
    req.password = password;
    req.full_name = full_name ? full_name : "";

    api->registerUser(
        req,
        [ctx]() {
            if (ctx->void_callback) {
                ctx->void_callback(ctx->user_data);
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_logout(
    BizMapClientHandle client,
    BizMapVoidCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->void_callback = on_success;
    ctx->error_callback = on_error;

    api->logout(
        [ctx]() {
            if (ctx->void_callback) {
                ctx->void_callback(ctx->user_data);
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_get_profile(
    BizMapClientHandle client,
    BizMapProfileCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->profile_callback = on_success;
    ctx->error_callback = on_error;

    api->getProfile(
        [ctx](const User& user) {
            if (ctx->profile_callback) {
                // TODO: Convertir User a JSON string
                std::string json = "{}"; // Placeholder
                ctx->profile_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

// ========== Búsqueda de Productos ==========

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
) {
    if (!client || !query) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->products_callback = on_success;
    ctx->error_callback = on_error;

    ProductSearchParams params;
    params.query = query;

    if (latitude != 0.0 || longitude != 0.0) {
        params.location = GeoLocation(latitude, longitude);
    }

    if (radius_meters > 0) {
        params.radius_meters = radius_meters;
    }

    if (min_price >= 0) {
        params.min_price = min_price;
    }

    if (max_price >= 0) {
        params.max_price = max_price;
    }

    if (category) {
        params.category = category;
    }

    params.limit = limit > 0 ? limit : 20;
    params.offset = offset >= 0 ? offset : 0;

    api->searchProducts(
        params,
        [ctx](const std::vector<ProductWithBusiness>& products) {
            if (ctx->products_callback) {
                // TODO: Convertir productos a JSON string
                std::string json = "[]"; // Placeholder
                ctx->products_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_get_product(
    BizMapClientHandle client,
    const char* product_id,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client || !product_id) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->products_callback = on_success;
    ctx->error_callback = on_error;

    api->getProductById(
        product_id,
        [ctx](const Product& product) {
            if (ctx->products_callback) {
                // TODO: Convertir producto a JSON string
                std::string json = "{}"; // Placeholder
                ctx->products_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

// ========== Búsqueda de Negocios ==========

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
) {
    if (!client) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->businesses_callback = on_success;
    ctx->error_callback = on_error;

    BusinessSearchParams params;
    params.query = query ? query : "";

    if (latitude != 0.0 || longitude != 0.0) {
        params.location = GeoLocation(latitude, longitude);
    }

    if (radius_meters > 0) {
        params.radius_meters = radius_meters;
    }

    if (category) {
        params.category = category;
    }

    params.limit = limit > 0 ? limit : 20;
    params.offset = offset >= 0 ? offset : 0;

    api->searchBusinesses(
        params,
        [ctx](const std::vector<Business>& businesses) {
            if (ctx->businesses_callback) {
                // TODO: Convertir negocios a JSON string
                std::string json = "[]"; // Placeholder
                ctx->businesses_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_get_business(
    BizMapClientHandle client,
    const char* business_id,
    BizMapBusinessesCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client || !business_id) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->businesses_callback = on_success;
    ctx->error_callback = on_error;

    api->getBusinessById(
        business_id,
        [ctx](const Business& business) {
            if (ctx->businesses_callback) {
                // TODO: Convertir negocio a JSON string
                std::string json = "{}"; // Placeholder
                ctx->businesses_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_get_business_products(
    BizMapClientHandle client,
    const char* business_id,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client || !business_id) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->products_callback = on_success;
    ctx->error_callback = on_error;

    api->getBusinessProducts(
        business_id,
        [ctx](const std::vector<ProductWithBusiness>& products) {
            if (ctx->products_callback) {
                // TODO: Convertir productos a JSON string
                std::string json = "[]"; // Placeholder
                ctx->products_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

// ========== Historial y Recomendaciones ==========

void bizmap_get_search_history(
    BizMapClientHandle client,
    int limit,
    BizMapProductsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->products_callback = on_success;
    ctx->error_callback = on_error;

    api->getSearchHistory(
        limit > 0 ? limit : 20,
        [ctx](const std::vector<SearchHistoryEntry>& history) {
            if (ctx->products_callback) {
                // TODO: Convertir historial a JSON string
                std::string json = "[]"; // Placeholder
                ctx->products_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_get_recommendations(
    BizMapClientHandle client,
    BizMapRecommendationsCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->recommendations_callback = on_success;
    ctx->error_callback = on_error;

    api->getRecommendations(
        [ctx](const UserRecommendations& recs) {
            if (ctx->recommendations_callback) {
                // TODO: Convertir recomendaciones a JSON string
                std::string json = "{}"; // Placeholder
                ctx->recommendations_callback(ctx->user_data, json.c_str());
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

void bizmap_clear_search_history(
    BizMapClientHandle client,
    BizMapVoidCallback on_success,
    BizMapErrorCallback on_error,
    void* user_data
) {
    if (!client) return;

    auto* api = static_cast<ApiClient*>(client);
    auto* ctx = new CallbackContext{user_data};
    ctx->void_callback = on_success;
    ctx->error_callback = on_error;

    api->clearSearchHistory(
        [ctx]() {
            if (ctx->void_callback) {
                ctx->void_callback(ctx->user_data);
            }
            delete ctx;
        },
        [ctx](const ApiError& error) {
            if (ctx->error_callback) {
                ctx->error_callback(
                    ctx->user_data,
                    error.status_code,
                    error.error_message.c_str()
                );
            }
            delete ctx;
        }
    );
}

// ========== Utilidades ==========

const char* bizmap_get_version(void) {
    static std::string version = bizmap::getVersion();
    return version.c_str();
}
