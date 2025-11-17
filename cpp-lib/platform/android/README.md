# Integración con Android (Kotlin/Java)

Esta guía explica cómo integrar la biblioteca BizMap en una aplicación Android usando JNI.

## Requisitos

- Android NDK 21 o superior
- Gradle 7.0+
- Kotlin 1.7+ o Java 8+

## Paso 1: Compilar la Biblioteca Nativa

### Usando CMake en Android Studio

Agregar en `app/build.gradle`:

```gradle
android {
    defaultConfig {
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++17"
                arguments "-DBIZMAP_BUILD_SHARED=ON"
            }
        }
        ndk {
            abiFilters 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64'
        }
    }

    externalNativeBuild {
        cmake {
            path file('../../cpp-lib/CMakeLists.txt')
            version '3.18.1'
        }
    }
}
```

## Paso 2: Crear Wrapper Kotlin

Crear `BizMapClient.kt`:

```kotlin
package com.aisoldev.bizmap

class BizMapClient(baseUrl: String) {
    private var nativeHandle: Long = 0

    init {
        System.loadLibrary("bizmap")
        nativeHandle = nativeCreateClient(baseUrl)
    }

    fun setAuthToken(token: String) {
        nativeSetAuthToken(nativeHandle, token)
    }

    fun login(
        username: String,
        password: String,
        onSuccess: (accessToken: String, refreshToken: String, expiresIn: Int) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        nativeLogin(nativeHandle, username, password, onSuccess, onError)
    }

    fun searchProducts(
        query: String,
        latitude: Double,
        longitude: Double,
        radiusMeters: Int = 5000,
        minPrice: Double = -1.0,
        maxPrice: Double = -1.0,
        category: String? = null,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: (productsJson: String) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        nativeSearchProducts(
            nativeHandle, query, latitude, longitude,
            radiusMeters, minPrice, maxPrice,
            category, limit, offset,
            onSuccess, onError
        )
    }

    fun searchBusinesses(
        query: String,
        latitude: Double,
        longitude: Double,
        radiusMeters: Int = 5000,
        category: String? = null,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: (businessesJson: String) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        nativeSearchBusinesses(
            nativeHandle, query, latitude, longitude,
            radiusMeters, category, limit, offset,
            onSuccess, onError
        )
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroyClient(nativeHandle)
            nativeHandle = 0
        }
    }

    protected fun finalize() {
        destroy()
    }

    // Métodos nativos
    private external fun nativeCreateClient(baseUrl: String): Long
    private external fun nativeDestroyClient(handle: Long)
    private external fun nativeSetAuthToken(handle: Long, token: String)

    private external fun nativeLogin(
        handle: Long,
        username: String,
        password: String,
        onSuccess: (String, String, Int) -> Unit,
        onError: (Int, String) -> Unit
    )

    private external fun nativeSearchProducts(
        handle: Long,
        query: String,
        latitude: Double,
        longitude: Double,
        radiusMeters: Int,
        minPrice: Double,
        maxPrice: Double,
        category: String?,
        limit: Int,
        offset: Int,
        onSuccess: (String) -> Unit,
        onError: (Int, String) -> Unit
    )

    private external fun nativeSearchBusinesses(
        handle: Long,
        query: String,
        latitude: Double,
        longitude: Double,
        radiusMeters: Int,
        category: String?,
        limit: Int,
        offset: Int,
        onSuccess: (String) -> Unit,
        onError: (Int, String) -> Unit
    )
}
```

## Paso 3: Implementar JNI Bridge

Crear `jni_bridge.cpp` en `platform/android/`:

```cpp
#include <jni.h>
#include <bizmap/bizmap_c_api.h>
#include <string>
#include <memory>

// Helpers para convertir entre JNI y C++
extern "C" {

// Convertir jstring a const char*
const char* jstring_to_cstr(JNIEnv* env, jstring jstr) {
    if (!jstr) return nullptr;
    return env->GetStringUTFChars(jstr, nullptr);
}

// Liberar string
void release_cstr(JNIEnv* env, jstring jstr, const char* cstr) {
    if (jstr && cstr) {
        env->ReleaseStringUTFChars(jstr, cstr);
    }
}

// Crear cliente
JNIEXPORT jlong JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeCreateClient(
    JNIEnv* env,
    jobject /* this */,
    jstring baseUrl
) {
    const char* url = jstring_to_cstr(env, baseUrl);
    BizMapClientHandle client = bizmap_create_client(url);
    release_cstr(env, baseUrl, url);
    return reinterpret_cast<jlong>(client);
}

// Destruir cliente
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeDestroyClient(
    JNIEnv* /* env */,
    jobject /* this */,
    jlong handle
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);
    bizmap_destroy_client(client);
}

// Establecer token
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeSetAuthToken(
    JNIEnv* env,
    jobject /* this */,
    jlong handle,
    jstring token
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);
    const char* t = jstring_to_cstr(env, token);
    bizmap_set_auth_token(client, t);
    release_cstr(env, token, t);
}

// Estructura para callbacks de Java
struct JavaCallbackContext {
    JavaVM* jvm;
    jobject callback_success;
    jobject callback_error;
};

// Login
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeLogin(
    JNIEnv* env,
    jobject /* this */,
    jlong handle,
    jstring username,
    jstring password,
    jobject onSuccess,
    jobject onError
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);

    // Guardar referencias globales a los callbacks
    JavaVM* jvm;
    env->GetJavaVM(&jvm);

    auto* ctx = new JavaCallbackContext{
        jvm,
        env->NewGlobalRef(onSuccess),
        env->NewGlobalRef(onError)
    };

    const char* user = jstring_to_cstr(env, username);
    const char* pass = jstring_to_cstr(env, password);

    bizmap_login(
        client,
        user,
        pass,
        // Success callback
        [](void* user_data, const char* access_token, const char* refresh_token, int expires_in) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            // Llamar al callback de Kotlin
            jclass callbackClass = env->GetObjectClass(ctx->callback_success);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Ljava/lang/String;Ljava/lang/String;I)V");

            jstring jAccessToken = env->NewStringUTF(access_token);
            jstring jRefreshToken = env->NewStringUTF(refresh_token);

            env->CallVoidMethod(ctx->callback_success, invokeMethod, jAccessToken, jRefreshToken, expires_in);

            // Limpiar
            env->DeleteLocalRef(jAccessToken);
            env->DeleteLocalRef(jRefreshToken);
            env->DeleteGlobalRef(ctx->callback_success);
            env->DeleteGlobalRef(ctx->callback_error);
            delete ctx;
        },
        // Error callback
        [](void* user_data, int status_code, const char* error_message) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_error);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(ILjava/lang/String;)V");

            jstring jMessage = env->NewStringUTF(error_message);

            env->CallVoidMethod(ctx->callback_error, invokeMethod, status_code, jMessage);

            env->DeleteLocalRef(jMessage);
            env->DeleteGlobalRef(ctx->callback_success);
            env->DeleteGlobalRef(ctx->callback_error);
            delete ctx;
        },
        ctx
    );

    release_cstr(env, username, user);
    release_cstr(env, password, pass);
}

// TODO: Implementar los demás métodos nativos siguiendo el mismo patrón

} // extern "C"
```

## Paso 4: Uso en la Aplicación Android

```kotlin
class MainActivity : AppCompatActivity() {
    private lateinit var bizMapClient: BizMapClient

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Crear cliente
        bizMapClient = BizMapClient("http://192.168.1.100:8080")

        // Login
        bizMapClient.login(
            username = "testuser",
            password = "password123",
            onSuccess = { accessToken, refreshToken, expiresIn ->
                Log.i("BizMap", "Login exitoso! Token expira en $expiresIn segundos")
                bizMapClient.setAuthToken(accessToken)

                // Ahora buscar productos
                searchProducts()
            },
            onError = { statusCode, message ->
                Log.e("BizMap", "Error en login: $message (código $statusCode)")
            }
        )
    }

    private fun searchProducts() {
        bizMapClient.searchProducts(
            query = "laptop",
            latitude = -12.0464,
            longitude = -77.0428,
            radiusMeters = 5000,
            minPrice = 500.0,
            maxPrice = 2000.0,
            category = "Electrónica",
            onSuccess = { productsJson ->
                // Parsear JSON y mostrar productos
                val products = JSONArray(productsJson)
                Log.i("BizMap", "Encontrados ${products.length()} productos")

                for (i in 0 until products.length()) {
                    val product = products.getJSONObject(i)
                    Log.i("BizMap", "  ${product.getString("name")} - $${product.getDouble("price")}")
                }
            },
            onError = { statusCode, message ->
                Log.e("BizMap", "Error en búsqueda: $message")
            }
        )
    }

    override fun onDestroy() {
        super.onDestroy()
        bizMapClient.destroy()
    }
}
```

## Compilación

```bash
# Desde el directorio raíz del proyecto Android
./gradlew assembleDebug

# O en Android Studio:
# Build > Make Project
```

La biblioteca nativa se compilará automáticamente para todas las arquitecturas especificadas en `abiFilters`.
