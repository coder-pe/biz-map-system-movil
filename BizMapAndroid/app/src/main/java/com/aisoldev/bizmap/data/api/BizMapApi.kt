package com.aisoldev.bizmap.data.api

import com.aisoldev.bizmap.data.models.*
import retrofit2.http.*

interface BizMapApi {

    // ========== Autenticación ==========

    @POST("auth/login")
    suspend fun login(@Body request: LoginRequest): AuthResponse

    @POST("auth/register")
    suspend fun register(@Body request: RegisterRequest)

    @POST("auth/logout")
    suspend fun logout()

    @GET("auth/profile")
    suspend fun getProfile(): User

    // ========== Negocios ==========

    @GET("businesses/search")
    suspend fun searchBusinesses(
        @Query("q") query: String,
        @Query("latitude") latitude: Double? = null,
        @Query("longitude") longitude: Double? = null,
        @Query("radius_meters") radiusMeters: Int? = null,
        @Query("category") category: String? = null,
        @Query("limit") limit: Int = 20,
        @Query("offset") offset: Int = 0
    ): List<Business>

    @GET("businesses/{id}")
    suspend fun getBusinessById(@Path("id") businessId: String): Business

    // ========== Productos ==========

    @GET("products/search")
    suspend fun searchProducts(
        @Query("q") query: String,
        @Query("latitude") latitude: Double? = null,
        @Query("longitude") longitude: Double? = null,
        @Query("radius_meters") radiusMeters: Int? = null,
        @Query("min_price") minPrice: Double? = null,
        @Query("max_price") maxPrice: Double? = null,
        @Query("category") category: String? = null,
        @Query("limit") limit: Int = 20,
        @Query("offset") offset: Int = 0
    ): List<ProductWithBusiness>

    @GET("products/{id}")
    suspend fun getProductById(@Path("id") productId: String): Product

    @GET("businesses/{id}/products")
    suspend fun getBusinessProducts(@Path("id") businessId: String): List<Product>

    // ========== Historial y Recomendaciones ==========

    @GET("history/search")
    suspend fun getSearchHistory(@Query("limit") limit: Int = 20): List<SearchHistoryEntry>

    @GET("recommendations")
    suspend fun getRecommendations(): UserRecommendations

    @DELETE("history/search")
    suspend fun clearSearchHistory()
}
