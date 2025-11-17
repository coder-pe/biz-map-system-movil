# Integración con iOS (Swift)

Esta guía explica cómo integrar la biblioteca BizMap en una aplicación iOS usando Swift.

## Requisitos

- Xcode 13.0 o superior
- iOS 13.0 o superior
- Swift 5.5+

## Paso 1: Compilar la Biblioteca

### Usando Xcode

1. Crear un proyecto Xcode para compilar el framework:

```bash
cd cpp-lib
mkdir build-ios
cd build-ios

cmake .. \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBIZMAP_BUILD_SHARED=ON

cmake --build .
```

2. Esto generará un framework `BizMap.framework` que puedes agregar a tu proyecto Xcode.

## Paso 2: Crear Bridging Header

Crear `BizMap-Bridging-Header.h`:

```objective-c
#ifndef BizMap_Bridging_Header_h
#define BizMap_Bridging_Header_h

#include <bizmap/bizmap_c_api.h>

#endif
```

Configurar en Xcode:
- Build Settings > Objective-C Bridging Header > `BizMap-Bridging-Header.h`

## Paso 3: Crear Wrapper Swift

Crear `BizMapClient.swift`:

```swift
import Foundation

/// Cliente Swift para la biblioteca BizMap
public class BizMapClient {
    private var nativeHandle: BizMapClientHandle?

    /// Inicializa el cliente con una URL base
    public init(baseUrl: String) {
        self.nativeHandle = bizmap_create_client(baseUrl)
    }

    deinit {
        if let handle = nativeHandle {
            bizmap_destroy_client(handle)
        }
    }

    /// Establece el token de autenticación
    public func setAuthToken(_ token: String) {
        guard let handle = nativeHandle else { return }
        bizmap_set_auth_token(handle, token)
    }

    /// Limpia el token de autenticación
    public func clearAuthToken() {
        guard let handle = nativeHandle else { return }
        bizmap_clear_auth_token(handle)
    }

    // MARK: - Autenticación

    /// Inicia sesión
    public func login(
        username: String,
        password: String,
        onSuccess: @escaping (String, String, Int) -> Void,
        onError: @escaping (Int, String) -> Void
    ) {
        guard let handle = nativeHandle else {
            onError(0, "Cliente no inicializado")
            return
        }

        // Crear contexto para los callbacks
        let context = CallbackContext(
            successCallback: onSuccess,
            errorCallback: onError
        )
        let contextPtr = Unmanaged.passRetained(context).toOpaque()

        bizmap_login(
            handle,
            username,
            password,
            // Success callback
            { (userData, accessToken, refreshToken, expiresIn) in
                guard let userData = userData,
                      let accessToken = accessToken,
                      let refreshToken = refreshToken else { return }

                let context = Unmanaged<CallbackContext<(String, String, Int), (Int, String)>>.fromOpaque(userData).takeRetainedValue()

                let access = String(cString: accessToken)
                let refresh = String(cString: refreshToken)

                DispatchQueue.main.async {
                    context.successCallback?(access, refresh, Int(expiresIn))
                }
            },
            // Error callback
            { (userData, statusCode, errorMessage) in
                guard let userData = userData,
                      let errorMessage = errorMessage else { return }

                let context = Unmanaged<CallbackContext<(String, String, Int), (Int, String)>>.fromOpaque(userData).takeRetainedValue()

                let message = String(cString: errorMessage)

                DispatchQueue.main.async {
                    context.errorCallback?(Int(statusCode), message)
                }
            },
            contextPtr
        )
    }

    /// Registra un nuevo usuario
    public func register(
        username: String,
        email: String,
        password: String,
        fullName: String,
        onSuccess: @escaping () -> Void,
        onError: @escaping (Int, String) -> Void
    ) {
        guard let handle = nativeHandle else {
            onError(0, "Cliente no inicializado")
            return
        }

        let context = VoidCallbackContext(
            successCallback: onSuccess,
            errorCallback: onError
        )
        let contextPtr = Unmanaged.passRetained(context).toOpaque()

        bizmap_register(
            handle,
            username,
            email,
            password,
            fullName,
            { userData in
                guard let userData = userData else { return }
                let context = Unmanaged<VoidCallbackContext>.fromOpaque(userData).takeRetainedValue()

                DispatchQueue.main.async {
                    context.successCallback?()
                }
            },
            { (userData, statusCode, errorMessage) in
                guard let userData = userData,
                      let errorMessage = errorMessage else { return }

                let context = Unmanaged<VoidCallbackContext>.fromOpaque(userData).takeRetainedValue()
                let message = String(cString: errorMessage)

                DispatchQueue.main.async {
                    context.errorCallback?(Int(statusCode), message)
                }
            },
            contextPtr
        )
    }

    // MARK: - Búsqueda

    /// Busca productos
    public func searchProducts(
        query: String,
        latitude: Double = 0.0,
        longitude: Double = 0.0,
        radiusMeters: Int = 5000,
        minPrice: Double = -1.0,
        maxPrice: Double = -1.0,
        category: String? = nil,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: @escaping (String) -> Void,
        onError: @escaping (Int, String) -> Void
    ) {
        guard let handle = nativeHandle else {
            onError(0, "Cliente no inicializado")
            return
        }

        let context = StringCallbackContext(
            successCallback: onSuccess,
            errorCallback: onError
        )
        let contextPtr = Unmanaged.passRetained(context).toOpaque()

        bizmap_search_products(
            handle,
            query,
            latitude,
            longitude,
            Int32(radiusMeters),
            minPrice,
            maxPrice,
            category,
            Int32(limit),
            Int32(offset),
            { (userData, productsJson) in
                guard let userData = userData,
                      let productsJson = productsJson else { return }

                let context = Unmanaged<StringCallbackContext>.fromOpaque(userData).takeRetainedValue()
                let json = String(cString: productsJson)

                DispatchQueue.main.async {
                    context.successCallback?(json)
                }
            },
            { (userData, statusCode, errorMessage) in
                guard let userData = userData,
                      let errorMessage = errorMessage else { return }

                let context = Unmanaged<StringCallbackContext>.fromOpaque(userData).takeRetainedValue()
                let message = String(cString: errorMessage)

                DispatchQueue.main.async {
                    context.errorCallback?(Int(statusCode), message)
                }
            },
            contextPtr
        )
    }

    /// Busca negocios
    public func searchBusinesses(
        query: String,
        latitude: Double = 0.0,
        longitude: Double = 0.0,
        radiusMeters: Int = 5000,
        category: String? = nil,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: @escaping (String) -> Void,
        onError: @escaping (Int, String) -> Void
    ) {
        guard let handle = nativeHandle else {
            onError(0, "Cliente no inicializado")
            return
        }

        let context = StringCallbackContext(
            successCallback: onSuccess,
            errorCallback: onError
        )
        let contextPtr = Unmanaged.passRetained(context).toOpaque()

        bizmap_search_businesses(
            handle,
            query,
            latitude,
            longitude,
            Int32(radiusMeters),
            category,
            Int32(limit),
            Int32(offset),
            { (userData, businessesJson) in
                guard let userData = userData,
                      let businessesJson = businessesJson else { return }

                let context = Unmanaged<StringCallbackContext>.fromOpaque(userData).takeRetainedValue()
                let json = String(cString: businessesJson)

                DispatchQueue.main.async {
                    context.successCallback?(json)
                }
            },
            { (userData, statusCode, errorMessage) in
                guard let userData = userData,
                      let errorMessage = errorMessage else { return }

                let context = Unmanaged<StringCallbackContext>.fromOpaque(userData).takeRetainedValue()
                let message = String(cString: errorMessage)

                DispatchQueue.main.async {
                    context.errorCallback?(Int(statusCode), message)
                }
            },
            contextPtr
        )
    }
}

// MARK: - Contextos de Callback

private class CallbackContext<S, E> {
    var successCallback: ((S) -> Void)?
    var errorCallback: ((E) -> Void)?

    init(successCallback: ((S) -> Void)?, errorCallback: ((E) -> Void)?) {
        self.successCallback = successCallback
        self.errorCallback = errorCallback
    }
}

private class VoidCallbackContext {
    var successCallback: (() -> Void)?
    var errorCallback: ((Int, String) -> Void)?

    init(successCallback: (() -> Void)?, errorCallback: ((Int, String) -> Void)?) {
        self.successCallback = successCallback
        self.errorCallback = errorCallback
    }
}

private class StringCallbackContext {
    var successCallback: ((String) -> Void)?
    var errorCallback: ((Int, String) -> Void)?

    init(successCallback: ((String) -> Void)?, errorCallback: ((Int, String) -> Void)?) {
        self.successCallback = successCallback
        self.errorCallback = errorCallback
    }
}
```

## Paso 4: Uso en la Aplicación iOS

```swift
import UIKit

class ViewController: UIViewController {
    private var bizMapClient: BizMapClient!

    override func viewDidLoad() {
        super.viewDidLoad()

        // Crear cliente
        bizMapClient = BizMapClient(baseUrl: "http://192.168.1.100:8080")

        // Login
        bizMapClient.login(
            username: "testuser",
            password: "password123",
            onSuccess: { [weak self] (accessToken, refreshToken, expiresIn) in
                print("✓ Login exitoso! Token expira en \(expiresIn) segundos")
                self?.bizMapClient.setAuthToken(accessToken)

                // Buscar productos
                self?.searchProducts()
            },
            onError: { (statusCode, message) in
                print("✗ Error en login: \(message) (código \(statusCode))")
            }
        )
    }

    private func searchProducts() {
        bizMapClient.searchProducts(
            query: "laptop",
            latitude: -12.0464,
            longitude: -77.0428,
            radiusMeters: 5000,
            minPrice: 500.0,
            maxPrice: 2000.0,
            category: "Electrónica",
            onSuccess: { productsJson in
                print("✓ Productos encontrados:")
                print(productsJson)

                // Parsear JSON
                if let data = productsJson.data(using: .utf8),
                   let products = try? JSONSerialization.jsonObject(with: data) as? [[String: Any]] {
                    for product in products {
                        if let name = product["name"] as? String,
                           let price = product["price"] as? Double {
                            print("  \(name) - $\(price)")
                        }
                    }
                }
            },
            onError: { (statusCode, message) in
                print("✗ Error en búsqueda: \(message)")
            }
        )
    }
}
```

## Paso 5: Configurar el Proyecto Xcode

1. **Agregar el Framework:**
   - Arrastra `BizMap.framework` a tu proyecto Xcode
   - Target > General > Frameworks, Libraries, and Embedded Content
   - Selecciona "Embed & Sign"

2. **Configurar Build Settings:**
   - Header Search Paths: `$(PROJECT_DIR)/cpp-lib/include`
   - Library Search Paths: `$(PROJECT_DIR)/cpp-lib/build-ios`

3. **Configurar Info.plist:**
   - Agregar permisos de red si es necesario

## Compilación

```bash
# Compilar para simulador
xcodebuild -scheme YourApp -sdk iphonesimulator

# Compilar para dispositivo
xcodebuild -scheme YourApp -sdk iphoneos
```

O simplemente usa Xcode para compilar el proyecto (Cmd+B).
