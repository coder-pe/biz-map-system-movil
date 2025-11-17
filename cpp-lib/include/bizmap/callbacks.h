// Archivo: callbacks.h
// Definiciones de callbacks para operaciones asíncronas
#pragma once

#include <functional>
#include <vector>
#include "models.h"

namespace bizmap {

// ========== Callbacks Genéricos ==========

template<typename T>
using SuccessCallback = std::function<void(const T&)>;

using ErrorCallback = std::function<void(const ApiError&)>;
using VoidSuccessCallback = std::function<void()>;

// ========== Callbacks Específicos ==========

// Autenticación
using LoginSuccessCallback = SuccessCallback<AuthResponse>;
using ProfileSuccessCallback = SuccessCallback<User>;

// Negocios
using BusinessSuccessCallback = SuccessCallback<Business>;
using BusinessListSuccessCallback = SuccessCallback<std::vector<Business>>;

// Productos
using ProductSuccessCallback = SuccessCallback<Product>;
using ProductListSuccessCallback = SuccessCallback<std::vector<ProductWithBusiness>>;

// Historial y recomendaciones
using SearchHistorySuccessCallback = SuccessCallback<std::vector<SearchHistoryEntry>>;
using RecommendationsSuccessCallback = SuccessCallback<UserRecommendations>;

} // namespace bizmap
