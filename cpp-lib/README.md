# BizMap C++ Library

Biblioteca C++ multiplataforma para el sistema de localización de productos y servicios (BizMap). Diseñada para integrarse con aplicaciones móviles en **Android (Kotlin/Java)** e **iOS (Swift)**.

## Características

- **C++17** - Código moderno y eficiente
- **Header-only dependencies** - nlohmann/json y cpp-httplib
- **Interfaz C** - Para interoperabilidad con JNI y Swift
- **Multiplataforma** - Android, iOS, Linux, macOS, Windows
- **Asíncrona** - Callbacks para operaciones no bloqueantes
- **Type-safe** - API C++ fuertemente tipada

## Arquitectura

La biblioteca está organizada en capas:

```
┌─────────────────────────────────────────┐
│   Aplicación (Kotlin/Swift)             │
├─────────────────────────────────────────┤
│   JNI Bridge / Swift Bridge             │
├─────────────────────────────────────────┤
│   Interfaz C (bizmap_c_api.h)           │
├─────────────────────────────────────────┤
│   API C++ (ApiClient)                   │
├─────────────────────────────────────────┤
│   HTTP Client (cpp-httplib)             │
├─────────────────────────────────────────┤
│   Backend REST API (Go)                 │
└─────────────────────────────────────────┘
```

## Estructura del Proyecto

```
cpp-lib/
├── include/bizmap/          # Headers públicos
│   ├── models.h             # Estructuras de datos
│   ├── callbacks.h          # Definiciones de callbacks
│   ├── api_client.h         # Cliente API principal
│   ├── bizmap.h             # Header principal
│   └── bizmap_c_api.h       # Interfaz C para JNI/Swift
├── src/                     # Implementación
│   ├── api_client.cpp
│   ├── http_client.cpp
│   ├── json_converters.cpp
│   └── bizmap_c_api.cpp
├── platform/                # Integraciones específicas
│   ├── android/             # JNI para Android
│   └── ios/                 # Bridge para iOS
├── examples/                # Ejemplos de uso
│   ├── basic_example.cpp    # API C++
│   └── c_api_example.c      # Interfaz C
├── third_party/             # Dependencias
│   ├── httplib.h            # cpp-httplib
│   └── nlohmann/json.hpp    # nlohmann/json
└── CMakeLists.txt           # Sistema de compilación
```

## Requisitos

### Dependencias

- **C++17** o superior
- **CMake 3.14+**
- **nlohmann/json** 3.11.2+ (header-only)
- **cpp-httplib** 0.14.0+ (header-only)
- **OpenSSL** (opcional, para HTTPS)

### Plataformas Soportadas

- **Android** - API 21+ (NDK 21+)
- **iOS** - iOS 13.0+ (Xcode 13+)
- **Linux** - GCC 7+ / Clang 6+
- **macOS** - Xcode 11+
- **Windows** - MSVC 2019+ / MinGW

## Instalación

### 1. Descargar Dependencias

```bash
cd cpp-lib/third_party

# Opción A: Descarga manual
curl -O https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
curl -O https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
mkdir -p nlohmann && mv json.hpp nlohmann/

# Opción B: Git submodules
cd ..
git submodule add https://github.com/yhirose/cpp-httplib.git third_party/cpp-httplib
git submodule add https://github.com/nlohmann/json.git third_party/nlohmann-json
```

### 2. Compilar (Desktop/Testing)

```bash
cd cpp-lib
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Ejecutar ejemplos
./basic_example
./c_api_example
```

### 3. Compilar para Android

Ver documentación detallada en [`platform/android/README.md`](platform/android/README.md)

```bash
# Configurar en app/build.gradle de tu proyecto Android
android {
    externalNativeBuild {
        cmake {
            path file('../../cpp-lib/CMakeLists.txt')
        }
    }
}
```

### 4. Compilar para iOS

Ver documentación detallada en [`platform/ios/README.md`](platform/ios/README.md)

```bash
cd cpp-lib
mkdir build-ios && cd build-ios

cmake .. \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    -DBIZMAP_BUILD_SHARED=ON

cmake --build .
```

## Uso

### API C++

```cpp
#include <bizmap/bizmap.h>
using namespace bizmap;

// Crear cliente
ApiClient client("http://192.168.1.100:8080");

// Login
client.login("username", "password",
    [&](const AuthResponse& response) {
        std::cout << "Token: " << response.access_token << "\n";
        client.setAuthToken(response.access_token);
    },
    [](const ApiError& error) {
        std::cerr << "Error: " << error.error_message << "\n";
    }
);

// Buscar productos
ProductSearchParams params;
params.query = "laptop";
params.location = GeoLocation(-12.0464, -77.0428);
params.radius_meters = 5000;
params.min_price = 500.0;
params.max_price = 2000.0;

client.searchProducts(params,
    [](const std::vector<ProductWithBusiness>& products) {
        for (const auto& p : products) {
            std::cout << p.product.name << " - $" << p.product.price << "\n";
        }
    },
    [](const ApiError& error) {
        std::cerr << "Error: " << error.error_message << "\n";
    }
);
```

### Interfaz C (para JNI/Swift)

```c
#include <bizmap/bizmap_c_api.h>

// Crear cliente
BizMapClientHandle client = bizmap_create_client("http://localhost:8080");

// Login
bizmap_login(
    client,
    "username",
    "password",
    on_login_success,  // Callback
    on_error,          // Callback
    user_data
);

// Buscar productos
bizmap_search_products(
    client,
    "laptop",          // query
    -12.0464,          // latitude
    -77.0428,          // longitude
    5000,              // radius_meters
    500.0,             // min_price
    2000.0,            // max_price
    "Electrónica",     // category
    20,                // limit
    0,                 // offset
    on_products_found, // Callback
    on_error,          // Callback
    user_data
);

// Limpiar
bizmap_destroy_client(client);
```

## Ejemplos

### Android (Kotlin)

```kotlin
val client = BizMapClient("http://192.168.1.100:8080")

client.login("user", "pass",
    onSuccess = { token, refresh, expiresIn ->
        Log.i("BizMap", "Login OK! Token: $token")
        client.setAuthToken(token)
    },
    onError = { code, msg ->
        Log.e("BizMap", "Error: $msg")
    }
)

client.searchProducts(
    query = "laptop",
    latitude = -12.0464,
    longitude = -77.0428,
    radiusMeters = 5000,
    onSuccess = { json ->
        val products = JSONArray(json)
        // Mostrar productos...
    },
    onError = { code, msg ->
        Log.e("BizMap", "Error: $msg")
    }
)
```

### iOS (Swift)

```swift
let client = BizMapClient(baseUrl: "http://192.168.1.100:8080")

client.login(username: "user", password: "pass",
    onSuccess: { token, refresh, expiresIn in
        print("Login OK! Token: \(token)")
        client.setAuthToken(token)
    },
    onError: { code, msg in
        print("Error: \(msg)")
    }
)

client.searchProducts(
    query: "laptop",
    latitude: -12.0464,
    longitude: -77.0428,
    radiusMeters: 5000,
    onSuccess: { json in
        // Parsear y mostrar productos...
    },
    onError: { code, msg in
        print("Error: \(msg)")
    }
)
```

## API Reference

### Autenticación

- `login()` - Iniciar sesión
- `registerUser()` - Registrar nuevo usuario
- `logout()` - Cerrar sesión
- `getProfile()` - Obtener perfil del usuario

### Búsqueda

- `searchProducts()` - Buscar productos con filtros
- `searchBusinesses()` - Buscar negocios cercanos
- `getProductById()` - Obtener producto por ID
- `getBusinessById()` - Obtener negocio por ID
- `getBusinessProducts()` - Productos de un negocio

### Historial

- `getSearchHistory()` - Historial de búsquedas
- `getRecommendations()` - Recomendaciones personalizadas
- `clearSearchHistory()` - Limpiar historial

## Configuración de CMake

### Opciones

- `BIZMAP_BUILD_SHARED` - Compilar como biblioteca compartida (default: ON)
- `BIZMAP_BUILD_EXAMPLES` - Compilar ejemplos (default: ON)
- `BIZMAP_ENABLE_SSL` - Habilitar HTTPS con OpenSSL (default: OFF)

### Ejemplo

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBIZMAP_BUILD_SHARED=ON \
    -DBIZMAP_ENABLE_SSL=ON \
    -DBIZMAP_BUILD_EXAMPLES=ON
```

## Testing

```bash
cd build
ctest --verbose
```

## Notas de Implementación

### Callbacks Asíncronos

Todas las operaciones de red son asíncronas. Los callbacks se ejecutan en un thread diferente al principal. En Android e iOS, los wrappers se encargan de despachar al thread principal.

### Gestión de Memoria

- **C++ API**: Usa RAII y smart pointers
- **C API**: El cliente debe llamar a `bizmap_destroy_client()` manualmente

### Thread Safety

El `ApiClient` NO es thread-safe. Crear una instancia por thread o usar sincronización externa.

### JSON

Los parsers JSON están incompletos en esta versión. Se debe implementar usando `nlohmann/json` para producción.

## Roadmap

- [ ] Implementar parsers JSON completos con nlohmann/json
- [ ] Agregar soporte para callbacks de progreso
- [ ] Implementar caché local
- [ ] Agregar tests unitarios (GoogleTest)
- [ ] Soporte para WebSocket (notificaciones push)
- [ ] Documentación API completa (Doxygen)

## Licencia

Este proyecto es parte del sistema BizMap.

## Contacto

Para preguntas o soporte sobre la biblioteca:
- Email: soporte@aisoldev.com
- Documentación: [En construcción]
