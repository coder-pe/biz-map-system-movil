package com.aisoldev.bizmap.data

import android.util.Log
import com.aisoldev.bizmap.data.api.AuthInterceptor
import com.aisoldev.bizmap.data.api.BizMapApi
import com.aisoldev.bizmap.data.models.*
import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import kotlinx.coroutines.*
import kotlinx.serialization.json.Json
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.HttpException
import retrofit2.Retrofit
import java.util.concurrent.TimeUnit

class BizMapClient(baseUrl: String) {

    private val json = Json {
        ignoreUnknownKeys = true
        coerceInputValues = true
        encodeDefaults = true
    }

    private val authInterceptor = AuthInterceptor()

    private val okHttpClient = OkHttpClient.Builder()
        .addInterceptor(authInterceptor)
        .addInterceptor(HttpLoggingInterceptor().apply {
            level = HttpLoggingInterceptor.Level.BODY
        })
        .connectTimeout(30, TimeUnit.SECONDS)
        .readTimeout(30, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    private val retrofit = Retrofit.Builder()
        .baseUrl(if (baseUrl.endsWith("/")) baseUrl else "$baseUrl/")
        .client(okHttpClient)
        .addConverterFactory(json.asConverterFactory("application/json".toMediaType()))
        .build()

    private val api: BizMapApi = retrofit.create(BizMapApi::class.java)

    private val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    // ========== Configuración ==========

    fun setAuthToken(token: String) {
        authInterceptor.token = token
        Log.d(TAG, "Auth token establecido")
    }

    fun clearAuthToken() {
        authInterceptor.token = null
        Log.d(TAG, "Auth token eliminado")
    }

    // ========== Autenticación ==========

    fun login(
        username: String,
        password: String,
        onSuccess: (accessToken: String, refreshToken: String, expiresIn: Int) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val response = api.login(LoginRequest(username, password))
                withContext(Dispatchers.Main) {
                    onSuccess(response.accessToken, response.refreshToken, response.expiresIn)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun registerUser(
        username: String,
        email: String,
        password: String,
        fullName: String,
        onSuccess: () -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                api.register(RegisterRequest(username, email, password, fullName))
                withContext(Dispatchers.Main) {
                    onSuccess()
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun logout(
        onSuccess: () -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                api.logout()
                clearAuthToken()
                withContext(Dispatchers.Main) {
                    onSuccess()
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun getProfile(
        onSuccess: (User) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val user = api.getProfile()
                withContext(Dispatchers.Main) {
                    onSuccess(user)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    // ========== Productos ==========

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
        onSuccess: (List<ProductWithBusiness>) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val products = api.searchProducts(
                    query = query,
                    latitude = if (latitude != 0.0) latitude else null,
                    longitude = if (longitude != 0.0) longitude else null,
                    radiusMeters = if (latitude != 0.0 && longitude != 0.0) radiusMeters else null,
                    minPrice = if (minPrice >= 0) minPrice else null,
                    maxPrice = if (maxPrice >= 0) maxPrice else null,
                    category = category?.takeIf { it.isNotEmpty() },
                    limit = limit,
                    offset = offset
                )
                withContext(Dispatchers.Main) {
                    onSuccess(products)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun getProductById(
        productId: String,
        onSuccess: (Product) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val product = api.getProductById(productId)
                withContext(Dispatchers.Main) {
                    onSuccess(product)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    // ========== Negocios ==========

    fun searchBusinesses(
        query: String,
        latitude: Double = 0.0,
        longitude: Double = 0.0,
        radiusMeters: Int = 5000,
        category: String? = null,
        limit: Int = 20,
        offset: Int = 0,
        onSuccess: (List<Business>) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val businesses = api.searchBusinesses(
                    query = query,
                    latitude = if (latitude != 0.0) latitude else null,
                    longitude = if (longitude != 0.0) longitude else null,
                    radiusMeters = if (latitude != 0.0 && longitude != 0.0) radiusMeters else null,
                    category = category?.takeIf { it.isNotEmpty() },
                    limit = limit,
                    offset = offset
                )
                withContext(Dispatchers.Main) {
                    onSuccess(businesses)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun getBusinessById(
        businessId: String,
        onSuccess: (Business) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val business = api.getBusinessById(businessId)
                withContext(Dispatchers.Main) {
                    onSuccess(business)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun getBusinessProducts(
        businessId: String,
        onSuccess: (List<Product>) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val products = api.getBusinessProducts(businessId)
                withContext(Dispatchers.Main) {
                    onSuccess(products)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    // ========== Historial y Recomendaciones ==========

    fun getSearchHistory(
        limit: Int = 20,
        onSuccess: (List<SearchHistoryEntry>) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val history = api.getSearchHistory(limit)
                withContext(Dispatchers.Main) {
                    onSuccess(history)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun getRecommendations(
        onSuccess: (UserRecommendations) -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                val recommendations = api.getRecommendations()
                withContext(Dispatchers.Main) {
                    onSuccess(recommendations)
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    fun clearSearchHistory(
        onSuccess: () -> Unit,
        onError: (statusCode: Int, message: String) -> Unit
    ) {
        scope.launch {
            try {
                api.clearSearchHistory()
                withContext(Dispatchers.Main) {
                    onSuccess()
                }
            } catch (e: Exception) {
                val (code, message) = parseError(e)
                withContext(Dispatchers.Main) {
                    onError(code, message)
                }
            }
        }
    }

    // ========== Lifecycle ==========

    fun destroy() {
        scope.cancel()
        Log.d(TAG, "Cliente destruido")
    }

    // ========== Utilidades ==========

    private fun parseError(e: Exception): Pair<Int, String> {
        return when (e) {
            is HttpException -> {
                val code = e.code()
                val message = e.message() ?: "Error HTTP $code"
                Log.e(TAG, "HTTP Error: $code - $message")
                code to message
            }
            else -> {
                val message = e.message ?: "Error desconocido"
                Log.e(TAG, "Error: $message", e)
                0 to message
            }
        }
    }

    companion object {
        private const val TAG = "BizMapClient"
    }
}
