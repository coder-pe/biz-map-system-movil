package com.aisoldev.bizmap

import android.util.Log

class BizMapClient(baseUrl: String) {
    private var nativeHandle: Long = 0

    init {
        try {
            System.loadLibrary("bizmap")
            nativeHandle = nativeCreateClient(baseUrl)
            Log.d(TAG, "BizMapClient inicializado con handle: $nativeHandle")
        } catch (e: UnsatisfiedLinkError) {
            Log.e(TAG, "Error cargando biblioteca nativa", e)
            throw e
        }
    }

    fun setAuthToken(token: String) {
        if (nativeHandle == 0L) {
            Log.e(TAG, "Cliente no inicializado")
            return
        }
        nativeSetAuthToken(nativeHandle, token)
    }

    fun clearAuthToken() {
        if (nativeHandle == 0L) return
        nativeClearAuthToken(nativeHandle)
    }

    fun login(
        username: String,
        password: String,
        onSuccess: (accessToken: String, refreshToken: String, expiresIn: Int) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        if (nativeHandle == 0L) {
            onError(0, "Cliente no inicializado")
            return
        }
        nativeLogin(nativeHandle, username, password, onSuccess, onError)
    }

    fun registerUser(
        username: String,
        email: String,
        password: String,
        fullName: String,
        onSuccess: () -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        if (nativeHandle == 0L) {
            onError(0, "Cliente no inicializado")
            return
        }
        nativeRegister(nativeHandle, username, email, password, fullName, onSuccess, onError)
    }

    fun searchProducts(
        query: String,
        latitude: Double = 0.0,
        longitude: Double = 0.0,
        radiusMeters: Int = 5000,
        minPrice: Double = -1.0,
        maxPrice: Double = -1.0,
        category: String? = null,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: (productsJson: String) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        if (nativeHandle == 0L) {
            onError(0, "Cliente no inicializado")
            return
        }
        nativeSearchProducts(
            nativeHandle, query, latitude, longitude,
            radiusMeters, minPrice, maxPrice,
            category ?: "", limit, offset,
            onSuccess, onError
        )
    }

    fun searchBusinesses(
        query: String,
        latitude: Double = 0.0,
        longitude: Double = 0.0,
        radiusMeters: Int = 5000,
        category: String? = null,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: (businessesJson: String) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        if (nativeHandle == 0L) {
            onError(0, "Cliente no inicializado")
            return
        }
        nativeSearchBusinesses(
            nativeHandle, query, latitude, longitude,
            radiusMeters, category ?: "", limit, offset,
            onSuccess, onError
        )
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroyClient(nativeHandle)
            nativeHandle = 0
            Log.d(TAG, "Cliente destruido")
        }
    }

    protected fun finalize() {
        destroy()
    }

    // Métodos nativos JNI
    private external fun nativeCreateClient(baseUrl: String): Long
    private external fun nativeDestroyClient(handle: Long)
    private external fun nativeSetAuthToken(handle: Long, token: String)
    private external fun nativeClearAuthToken(handle: Long)

    private external fun nativeLogin(
        handle: Long,
        username: String,
        password: String,
        onSuccess: (String, String, Int) -> Unit,
        onError: (Int, String) -> Unit
    )

    private external fun nativeRegister(
        handle: Long,
        username: String,
        email: String,
        password: String,
        fullName: String,
        onSuccess: () -> Unit,
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
        category: String,
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
        category: String,
        limit: Int,
        offset: Int,
        onSuccess: (String) -> Unit,
        onError: (Int, String) -> Unit
    )

    companion object {
        private const val TAG = "BizMapClient"
    }
}
