#include <jni.h>
#include <bizmap/bizmap_c_api.h>
#include <android/log.h>
#include <string>
#include <memory>

#define LOG_TAG "BizMapJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Estructura para mantener referencias a callbacks de Java
struct JavaCallbackContext {
    JavaVM* jvm;
    jobject callback_success;
    jobject callback_error;

    ~JavaCallbackContext() {
        JNIEnv* env;
        if (jvm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK) {
            if (callback_success) env->DeleteGlobalRef(callback_success);
            if (callback_error) env->DeleteGlobalRef(callback_error);
        }
    }
};

extern "C" {

// Crear cliente
JNIEXPORT jlong JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeCreateClient(
    JNIEnv* env,
    jobject /* this */,
    jstring baseUrl
) {
    const char* url = env->GetStringUTFChars(baseUrl, nullptr);
    LOGI("Creando cliente con URL: %s", url);

    BizMapClientHandle client = bizmap_create_client(url);

    env->ReleaseStringUTFChars(baseUrl, url);

    LOGI("Cliente creado: %p", client);
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
    LOGI("Destruyendo cliente: %p", client);
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
    const char* t = env->GetStringUTFChars(token, nullptr);

    bizmap_set_auth_token(client, t);

    env->ReleaseStringUTFChars(token, t);
}

// Limpiar token
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeClearAuthToken(
    JNIEnv* /* env */,
    jobject /* this */,
    jlong handle
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);
    bizmap_clear_auth_token(client);
}

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

    // Obtener JavaVM
    JavaVM* jvm;
    env->GetJavaVM(&jvm);

    // Crear contexto con referencias globales
    auto* ctx = new JavaCallbackContext{
        jvm,
        env->NewGlobalRef(onSuccess),
        env->NewGlobalRef(onError)
    };

    const char* user = env->GetStringUTFChars(username, nullptr);
    const char* pass = env->GetStringUTFChars(password, nullptr);

    LOGI("Iniciando login para usuario: %s", user);

    bizmap_login(
        client,
        user,
        pass,
        // Success callback
        [](void* user_data, const char* access_token, const char* refresh_token, int expires_in) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            LOGI("Login exitoso, expires_in: %d", expires_in);

            // Obtener el método invoke de la lambda (Kotlin Function3)
            jclass callbackClass = env->GetObjectClass(ctx->callback_success);
            jmethodID invokeMethod = env->GetMethodID(
                callbackClass,
                "invoke",
                "(Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"
            );

            jstring jAccessToken = env->NewStringUTF(access_token);
            jstring jRefreshToken = env->NewStringUTF(refresh_token);

            // Convertir int a Integer
            jclass integerClass = env->FindClass("java/lang/Integer");
            jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
            jobject jExpiresIn = env->NewObject(integerClass, integerConstructor, expires_in);

            env->CallObjectMethod(
                ctx->callback_success,
                invokeMethod,
                jAccessToken,
                jRefreshToken,
                jExpiresIn
            );

            env->DeleteLocalRef(jExpiresIn);
            env->DeleteLocalRef(integerClass);

            env->DeleteLocalRef(jAccessToken);
            env->DeleteLocalRef(jRefreshToken);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        // Error callback
        [](void* user_data, int status_code, const char* error_message) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            LOGE("Error en login: %d - %s", status_code, error_message);

            jclass callbackClass = env->GetObjectClass(ctx->callback_error);
            jmethodID invokeMethod = env->GetMethodID(
                callbackClass,
                "invoke",
                "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"
            );

            // Convertir int a Integer
            jclass integerClass = env->FindClass("java/lang/Integer");
            jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
            jobject jStatusCode = env->NewObject(integerClass, integerConstructor, status_code);

            jstring jMessage = env->NewStringUTF(error_message);

            env->CallObjectMethod(ctx->callback_error, invokeMethod, jStatusCode, jMessage);

            env->DeleteLocalRef(jStatusCode);
            env->DeleteLocalRef(integerClass);

            env->DeleteLocalRef(jMessage);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        ctx
    );

    env->ReleaseStringUTFChars(username, user);
    env->ReleaseStringUTFChars(password, pass);
}

// Registro
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeRegister(
    JNIEnv* env,
    jobject /* this */,
    jlong handle,
    jstring username,
    jstring email,
    jstring password,
    jstring fullName,
    jobject onSuccess,
    jobject onError
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);

    JavaVM* jvm;
    env->GetJavaVM(&jvm);

    auto* ctx = new JavaCallbackContext{
        jvm,
        env->NewGlobalRef(onSuccess),
        env->NewGlobalRef(onError)
    };

    const char* user = env->GetStringUTFChars(username, nullptr);
    const char* mail = env->GetStringUTFChars(email, nullptr);
    const char* pass = env->GetStringUTFChars(password, nullptr);
    const char* name = env->GetStringUTFChars(fullName, nullptr);

    bizmap_register(
        client, user, mail, pass, name,
        [](void* user_data) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_success);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "()Ljava/lang/Object;");

            env->CallObjectMethod(ctx->callback_success, invokeMethod);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        [](void* user_data, int status_code, const char* error_message) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_error);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

            // Convertir int a Integer
            jclass integerClass = env->FindClass("java/lang/Integer");
            jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
            jobject jStatusCode = env->NewObject(integerClass, integerConstructor, status_code);

            jstring jMessage = env->NewStringUTF(error_message);
            env->CallObjectMethod(ctx->callback_error, invokeMethod, jStatusCode, jMessage);

            env->DeleteLocalRef(jStatusCode);
            env->DeleteLocalRef(integerClass);
            env->DeleteLocalRef(jMessage);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        ctx
    );

    env->ReleaseStringUTFChars(username, user);
    env->ReleaseStringUTFChars(email, mail);
    env->ReleaseStringUTFChars(password, pass);
    env->ReleaseStringUTFChars(fullName, name);
}

// Búsqueda de productos
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeSearchProducts(
    JNIEnv* env,
    jobject /* this */,
    jlong handle,
    jstring query,
    jdouble latitude,
    jdouble longitude,
    jint radiusMeters,
    jdouble minPrice,
    jdouble maxPrice,
    jstring category,
    jint limit,
    jint offset,
    jobject onSuccess,
    jobject onError
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);

    JavaVM* jvm;
    env->GetJavaVM(&jvm);

    auto* ctx = new JavaCallbackContext{
        jvm,
        env->NewGlobalRef(onSuccess),
        env->NewGlobalRef(onError)
    };

    const char* q = env->GetStringUTFChars(query, nullptr);
    const char* cat = category ? env->GetStringUTFChars(category, nullptr) : nullptr;

    bizmap_search_products(
        client, q, latitude, longitude, radiusMeters,
        minPrice, maxPrice, cat, limit, offset,
        [](void* user_data, const char* products_json) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_success);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Ljava/lang/Object;)Ljava/lang/Object;");

            jstring jJson = env->NewStringUTF(products_json);
            env->CallObjectMethod(ctx->callback_success, invokeMethod, jJson);

            env->DeleteLocalRef(jJson);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        [](void* user_data, int status_code, const char* error_message) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_error);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

            // Convertir int a Integer
            jclass integerClass = env->FindClass("java/lang/Integer");
            jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
            jobject jStatusCode = env->NewObject(integerClass, integerConstructor, status_code);

            jstring jMessage = env->NewStringUTF(error_message);
            env->CallObjectMethod(ctx->callback_error, invokeMethod, jStatusCode, jMessage);

            env->DeleteLocalRef(jStatusCode);
            env->DeleteLocalRef(integerClass);
            env->DeleteLocalRef(jMessage);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        ctx
    );

    env->ReleaseStringUTFChars(query, q);
    if (cat) env->ReleaseStringUTFChars(category, cat);
}

// Búsqueda de negocios
JNIEXPORT void JNICALL
Java_com_aisoldev_bizmap_BizMapClient_nativeSearchBusinesses(
    JNIEnv* env,
    jobject /* this */,
    jlong handle,
    jstring query,
    jdouble latitude,
    jdouble longitude,
    jint radiusMeters,
    jstring category,
    jint limit,
    jint offset,
    jobject onSuccess,
    jobject onError
) {
    BizMapClientHandle client = reinterpret_cast<BizMapClientHandle>(handle);

    JavaVM* jvm;
    env->GetJavaVM(&jvm);

    auto* ctx = new JavaCallbackContext{
        jvm,
        env->NewGlobalRef(onSuccess),
        env->NewGlobalRef(onError)
    };

    const char* q = env->GetStringUTFChars(query, nullptr);
    const char* cat = category ? env->GetStringUTFChars(category, nullptr) : nullptr;

    bizmap_search_businesses(
        client, q, latitude, longitude, radiusMeters,
        cat, limit, offset,
        [](void* user_data, const char* businesses_json) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_success);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Ljava/lang/Object;)Ljava/lang/Object;");

            jstring jJson = env->NewStringUTF(businesses_json);
            env->CallObjectMethod(ctx->callback_success, invokeMethod, jJson);

            env->DeleteLocalRef(jJson);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        [](void* user_data, int status_code, const char* error_message) {
            auto* ctx = static_cast<JavaCallbackContext*>(user_data);
            JNIEnv* env;
            ctx->jvm->AttachCurrentThread(&env, nullptr);

            jclass callbackClass = env->GetObjectClass(ctx->callback_error);
            jmethodID invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

            // Convertir int a Integer
            jclass integerClass = env->FindClass("java/lang/Integer");
            jmethodID integerConstructor = env->GetMethodID(integerClass, "<init>", "(I)V");
            jobject jStatusCode = env->NewObject(integerClass, integerConstructor, status_code);

            jstring jMessage = env->NewStringUTF(error_message);
            env->CallObjectMethod(ctx->callback_error, invokeMethod, jStatusCode, jMessage);

            env->DeleteLocalRef(jStatusCode);
            env->DeleteLocalRef(integerClass);
            env->DeleteLocalRef(jMessage);
            env->DeleteLocalRef(callbackClass);

            delete ctx;
        },
        ctx
    );

    env->ReleaseStringUTFChars(query, q);
    if (cat) env->ReleaseStringUTFChars(category, cat);
}

} // extern "C"
