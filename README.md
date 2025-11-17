# BizMap - Aplicación Móvil (Biblioteca Nativa)

Este directorio contiene la **biblioteca C++ nativa** compartida para las aplicaciones móviles Android e iOS del sistema BizMap.

## ⚠️ Migración desde Qt

**Nota importante:** El código Qt/QML ha sido **completamente eliminado** de este proyecto. La aplicación móvil ahora utiliza aplicaciones nativas para cada plataforma:

- **Android**: Kotlin + Jetpack Compose / Views
- **iOS**: Swift + SwiftUI

Ambas plataformas comparten la misma **biblioteca C++** para la lógica de negocio y comunicación con el backend.

## Estructura del Directorio

```
biz-map-system-movil/
└── cpp-lib/                 # Biblioteca C++ compartida
    ├── include/bizmap/      # Headers públicos
    ├── src/                 # Implementación
    ├── platform/            # Integraciones específicas
    │   ├── android/         # JNI para Kotlin/Java
    │   └── ios/             # Bridge para Swift
    ├── examples/            # Ejemplos de uso
    ├── third_party/         # Dependencias
    └── CMakeLists.txt       # Sistema de compilación
```

## ¿Qué contiene la Biblioteca C++?

La biblioteca `libbizmap` proporciona:

- ✅ Cliente API REST para comunicación con el backend
- ✅ Modelos de datos (User, Business, Product, etc.)
- ✅ Manejo de ubicación geográfica
- ✅ Búsqueda de productos y negocios
- ✅ Autenticación JWT
- ✅ Historial de búsquedas y recomendaciones
- ✅ Interfaz C para JNI (Android) y Swift (iOS)

## Inicio Rápido

### Ver Documentación Completa

Para información detallada sobre cómo usar la biblioteca, consulta:

📖 **[cpp-lib/README.md](cpp-lib/README.md)** - Documentación principal de la biblioteca

### Guías de Integración por Plataforma

- 🤖 **Android (Kotlin/Java)**: [cpp-lib/platform/android/README.md](cpp-lib/platform/android/README.md)
- 🍎 **iOS (Swift)**: [cpp-lib/platform/ios/README.md](cpp-lib/platform/ios/README.md)

### Ejemplo Rápido - Android

```kotlin
// Crear cliente
val client = BizMapClient("http://192.168.1.100:8080")

// Login
client.login("username", "password",
    onSuccess = { token, refresh, expiresIn ->
        Log.i("BizMap", "Login exitoso!")
        client.setAuthToken(token)
    },
    onError = { code, msg ->
        Log.e("BizMap", "Error: $msg")
    }
)

// Buscar productos
client.searchProducts(
    query = "laptop",
    latitude = -12.0464,
    longitude = -77.0428,
    radiusMeters = 5000,
    onSuccess = { json ->
        // Procesar resultados...
    },
    onError = { code, msg ->
        Log.e("BizMap", "Error: $msg")
    }
)
```

### Ejemplo Rápido - iOS

```swift
// Crear cliente
let client = BizMapClient(baseUrl: "http://192.168.1.100:8080")

// Login
client.login(username: "username", password: "password",
    onSuccess: { token, refresh, expiresIn in
        print("Login exitoso!")
        client.setAuthToken(token)
    },
    onError: { code, msg in
        print("Error: \(msg)")
    }
)

// Buscar productos
client.searchProducts(
    query: "laptop",
    latitude: -12.0464,
    longitude: -77.0428,
    radiusMeters: 5000,
    onSuccess: { json in
        // Procesar resultados...
    },
    onError: { code, msg in
        print("Error: \(msg)")
    }
)
```

## Compilación

### Android

```bash
# En tu proyecto Android, configura build.gradle:
android {
    externalNativeBuild {
        cmake {
            path file('../../biz-map-system-movil/cpp-lib/CMakeLists.txt')
        }
    }
}

# Compilar
./gradlew assembleDebug
```

### iOS

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

### Desktop (para testing)

```bash
cd cpp-lib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Ejecutar ejemplos
./examples/basic_example
./examples/c_api_example
```

## Arquitectura

```
┌─────────────────────────────────────┐
│   Aplicación Android (Kotlin)       │
│   Aplicación iOS (Swift)            │
├─────────────────────────────────────┤
│   JNI Bridge / Swift Bridging       │
├─────────────────────────────────────┤
│   Interfaz C (bizmap_c_api.h)       │
├─────────────────────────────────────┤
│   Biblioteca C++ (libbizmap)        │
│   - ApiClient                       │
│   - Models                          │
│   - HttpClient                      │
├─────────────────────────────────────┤
│   Backend REST API (Go)             │
└─────────────────────────────────────┘
```

## Tecnologías

- **Lenguaje**: C++17
- **HTTP Client**: cpp-httplib (header-only)
- **JSON**: nlohmann/json (header-only)
- **Build System**: CMake 3.14+
- **Plataformas**: Android (API 21+), iOS (13.0+), Linux, macOS, Windows

## Dependencias

Las dependencias son **header-only** y deben descargarse en `cpp-lib/third_party/`:

1. **cpp-httplib** - Cliente HTTP
2. **nlohmann/json** - Parser JSON

Ver [cpp-lib/third_party/README.md](cpp-lib/third_party/README.md) para instrucciones de instalación.

## API Reference

### Autenticación
- `login()` - Iniciar sesión
- `registerUser()` - Registrar usuario
- `logout()` - Cerrar sesión
- `getProfile()` - Obtener perfil

### Búsqueda
- `searchProducts()` - Buscar productos con filtros
- `searchBusinesses()` - Buscar negocios cercanos
- `getProductById()` - Obtener producto
- `getBusinessById()` - Obtener negocio

### Historial
- `getSearchHistory()` - Historial de búsquedas
- `getRecommendations()` - Recomendaciones personalizadas
- `clearSearchHistory()` - Limpiar historial

## Estado del Proyecto

✅ **Completado:**
- Biblioteca C++ nativa funcional
- Interfaz C para JNI y Swift
- Sistema de compilación multiplataforma
- Ejemplos de integración
- Documentación completa

⚠️ **Pendiente:**
- Implementar parsers JSON completos con nlohmann/json
- Crear proyectos de ejemplo completos para Android e iOS
- Agregar tests unitarios

## Soporte

Para preguntas o problemas:
- Consulta la documentación en `cpp-lib/README.md`
- Revisa los ejemplos en `cpp-lib/examples/`
- Consulta las guías de integración en `cpp-lib/platform/`

## Historial de Cambios

### Versión 1.0.0 (Noviembre 2024)
- ❌ **Eliminado**: Todo el código Qt/QML
- ✅ **Agregado**: Biblioteca C++ nativa compartida
- ✅ **Agregado**: Soporte para Android (JNI)
- ✅ **Agregado**: Soporte para iOS (Swift bridging)
- ✅ **Agregado**: Sistema de compilación con CMake
- ✅ **Agregado**: Documentación completa

---

**Nota**: Si necesitas acceder al código Qt antiguo, puedes encontrarlo en el historial de git antes de este commit.
