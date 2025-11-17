# Guía de Migración: Qt/QML → C++ Nativo + Kotlin/Swift

Este documento describe la migración realizada de la aplicación móvil de Qt/QML a una arquitectura basada en C++ nativo con aplicaciones nativas para Android (Kotlin) e iOS (Swift).

## Resumen de Cambios

### ❌ Eliminado

El siguiente código y estructura ha sido **completamente eliminado**:

1. **Directorio `include/`** - Headers Qt antiguos:
   - `ApiClient.h` (versión Qt)
   - `MainWindow.h`
   - `BusinessMapSystem.h`
   - `SearchEngine.h`
   - `GeoLocation.h`
   - `UserProfile.h`
   - `BusinessProfile.h`
   - `ProductService.h`
   - `MapInterface.h`
   - `GoogleMapsInterface.h`

2. **Directorio `src/`** - Código fuente Qt:
   - `ApiClient.cpp` (versión Qt)
   - `MainWindow.cpp`
   - `SearchEngine.cpp`
   - `GoogleMapsInterface.cpp`
   - `main.cpp` (punto de entrada Qt)
   - `main.qml` (interfaz QML)
   - `qml.qrc` (recursos QML)
   - Todos los archivos compilados (`.o`)
   - Archivos generados por Qt (`moc_*`, `qrc_*`)
   - `Makefile` generado
   - `.pro` archivos de proyecto Qt
   - Directorio `BusinessMapLocator.app`

### ✅ Agregado/Mantenido

1. **Directorio `cpp-lib/`** - Nueva biblioteca C++ nativa:
   ```
   cpp-lib/
   ├── include/bizmap/      # API pública
   ├── src/                 # Implementación
   ├── platform/            # Integraciones JNI/Swift
   ├── examples/            # Ejemplos de uso
   ├── third_party/         # Dependencias
   └── CMakeLists.txt       # Build system
   ```

2. **README.md** - Documentación del directorio móvil

3. **MIGRATION.md** - Este documento

## Arquitectura Anterior vs Nueva

### Antes (Qt/QML)

```
┌────────────────────────────┐
│   Aplicación Qt            │
│   ┌────────────────────┐   │
│   │  QML (UI)          │   │
│   └────────────────────┘   │
│   ┌────────────────────┐   │
│   │  C++ (Logic)       │   │
│   │  - MainWindow      │   │
│   │  - ApiClient       │   │
│   │  - Qt Classes      │   │
│   └────────────────────┘   │
└────────────────────────────┘
```

**Problemas:**
- ❌ Dependencia pesada de Qt para aplicaciones móviles
- ❌ QML no es nativo en Android/iOS
- ❌ Limitaciones de rendimiento
- ❌ Tamaño grande del APK/IPA por Qt
- ❌ Difícil mantener look & feel nativo

### Ahora (C++ + Nativo)

```
┌─────────────────────────────────────┐
│   Android App (Kotlin)              │
│   ┌─────────────────────────────┐   │
│   │  Jetpack Compose (UI)       │   │
│   └─────────────────────────────┘   │
│   ┌─────────────────────────────┐   │
│   │  BizMapClient (Kotlin)      │   │
│   └─────────────────────────────┘   │
│   ┌─────────────────────────────┐   │
│   │  JNI Bridge (C/C++)         │   │
│   └─────────────────────────────┘   │
└─────────────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────┐
│   Biblioteca C++ (libbizmap.so)     │
│   - ApiClient                       │
│   - HttpClient                      │
│   - JSON parsing                    │
│   - Models                          │
└─────────────────────────────────────┘
                 ^
                 │
┌─────────────────────────────────────┐
│   iOS App (Swift)                   │
│   ┌─────────────────────────────┐   │
│   │  SwiftUI (UI)               │   │
│   └─────────────────────────────┘   │
│   ┌─────────────────────────────┐   │
│   │  BizMapClient (Swift)       │   │
│   └─────────────────────────────┘   │
│   ┌─────────────────────────────┐   │
│   │  C Bridging (C/C++)         │   │
│   └─────────────────────────────┘   │
└─────────────────────────────────────┘
```

**Ventajas:**
- ✅ Apps 100% nativas con look & feel de cada plataforma
- ✅ Mejor rendimiento
- ✅ Tamaño reducido de APK/IPA
- ✅ Código compartido en C++ para lógica de negocio
- ✅ Fácil acceso a APIs nativas de cada plataforma
- ✅ Mejores herramientas de desarrollo (Android Studio, Xcode)

## Mapeo de Componentes

### Antes → Ahora

| Componente Qt | Nuevo Equivalente | Plataforma |
|---------------|-------------------|------------|
| `MainWindow.h/.cpp` | `MainActivity.kt` / `ContentView.swift` | Android / iOS |
| `main.qml` | Jetpack Compose / SwiftUI | Android / iOS |
| `ApiClient` (Qt) | `ApiClient` (C++) + JNI/Swift Bridge | Compartido |
| `QNetworkAccessManager` | `cpp-httplib` | Compartido (C++) |
| `QJsonDocument` | `nlohmann/json` | Compartido (C++) |
| `QGeoCoordinate` | `LatLng` / `CLLocationCoordinate2D` | Android / iOS |
| Qt Maps | Google Maps SDK / MapKit | Android / iOS |
| Qt Signals/Slots | Callbacks (lambdas/closures) | Todos |

## Dependencias

### Antes

```
Qt 5.15+
├── QtCore
├── QtGui
├── QtQuick
├── QtQml
├── QtLocation
├── QtPositioning
└── QtNetworking
```

**Tamaño:** ~40-60 MB por plataforma

### Ahora

**Biblioteca C++ (`libbizmap`):**
```
C++17 Standard Library
├── cpp-httplib (header-only, ~200 KB)
├── nlohmann/json (header-only, ~100 KB)
└── CMake (build tool)
```

**Tamaño:** ~500 KB - 2 MB por plataforma

**Android App:**
```
Kotlin + Jetpack
├── Jetpack Compose
├── Google Maps SDK
├── Location Services
└── libbizmap.so (JNI)
```

**iOS App:**
```
Swift + SwiftUI
├── SwiftUI
├── MapKit
├── CoreLocation
└── BizMap.framework
```

## Guía de Migración para Desarrolladores

### Si tenías código Qt anterior

1. **UI/Interfaz**: Reescribir en Jetpack Compose (Android) o SwiftUI (iOS)
   - No hay migración automática
   - Beneficio: UI nativa y moderna

2. **Lógica de Negocio**: Si está en C++, moverla a `cpp-lib/src/`
   - Remover dependencias de Qt
   - Usar `std::` en lugar de `Q*` classes

3. **Networking**: La nueva `ApiClient` en C++ ya implementa todas las llamadas
   - Usar callbacks en lugar de signals/slots
   - Interfaz más simple y directa

### Creando una nueva App Android

Ver: [`cpp-lib/platform/android/README.md`](cpp-lib/platform/android/README.md)

```kotlin
// 1. Crear BizMapClient
val client = BizMapClient("http://192.168.1.100:8080")

// 2. Usar callbacks
client.login("user", "pass",
    onSuccess = { token, _, _ ->
        // Manejar éxito
    },
    onError = { code, msg ->
        // Manejar error
    }
)
```

### Creando una nueva App iOS

Ver: [`cpp-lib/platform/ios/README.md`](cpp-lib/platform/ios/README.md)

```swift
// 1. Crear BizMapClient
let client = BizMapClient(baseUrl: "http://192.168.1.100:8080")

// 2. Usar callbacks
client.login(username: "user", password: "pass",
    onSuccess: { token, _, _ in
        // Manejar éxito
    },
    onError: { code, msg in
        // Manejar error
    }
)
```

## Compilación

### Antes (Qt)

```bash
qmake
make
# O usar Qt Creator
```

### Ahora

**Biblioteca C++:**
```bash
cd cpp-lib
mkdir build && cd build
cmake ..
cmake --build .
```

**Android:**
```bash
# Configurado en build.gradle
./gradlew assembleDebug
```

**iOS:**
```bash
# Usar Xcode o
xcodebuild -scheme YourApp -sdk iphoneos
```

## Testing

### Antes
- Qt Test Framework
- QML Test

### Ahora
- **C++**: GoogleTest / Catch2
- **Android**: JUnit + Mockk
- **iOS**: XCTest

## Preguntas Frecuentes

### ¿Por qué eliminar Qt?

1. **Tamaño**: Qt agrega 40-60 MB a cada app
2. **Rendimiento**: QML es más lento que UI nativa
3. **Look & Feel**: Difícil mantener apariencia nativa
4. **Mantenimiento**: Requiere conocimiento de Qt, QML y C++
5. **Herramientas**: Android Studio y Xcode son superiores para desarrollo móvil

### ¿Qué pasa con el código Qt existente?

- Está en el historial de Git
- Se puede recuperar si es necesario
- La lógica de negocio fue migrada a C++ puro

### ¿Puedo usar la biblioteca en Desktop?

¡Sí! La biblioteca C++ funciona en:
- ✅ Linux
- ✅ macOS
- ✅ Windows
- ✅ Android
- ✅ iOS

Solo cambia el wrapper de UI.

### ¿Dónde está el código de las apps Android/iOS?

Aún no están creadas. Este directorio solo contiene la **biblioteca compartida**.

Los proyectos de apps nativas deben crearse en directorios separados:
- `android-app/` (proyecto Android Studio)
- `ios-app/` (proyecto Xcode)

Ambos enlazarán con `cpp-lib/`.

## Próximos Pasos

1. ✅ Biblioteca C++ completada
2. ⏳ Crear proyecto Android con Kotlin + Compose
3. ⏳ Crear proyecto iOS con Swift + SwiftUI
4. ⏳ Implementar UI en ambas plataformas
5. ⏳ Testing end-to-end

## Recursos

- **Documentación C++**: [cpp-lib/README.md](cpp-lib/README.md)
- **Integración Android**: [cpp-lib/platform/android/README.md](cpp-lib/platform/android/README.md)
- **Integración iOS**: [cpp-lib/platform/ios/README.md](cpp-lib/platform/ios/README.md)
- **Ejemplos de código**: [cpp-lib/examples/](cpp-lib/examples/)

## Soporte

Para preguntas sobre la migración o la nueva arquitectura:
- Revisa la documentación en `cpp-lib/`
- Consulta los ejemplos en `cpp-lib/examples/`
- Revisa las guías de integración por plataforma

---

**Fecha de migración**: Noviembre 2024
**Versión anterior**: Qt 5.15
**Versión nueva**: C++17 + Kotlin + Swift
