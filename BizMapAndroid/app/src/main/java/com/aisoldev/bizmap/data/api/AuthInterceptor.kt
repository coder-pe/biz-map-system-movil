package com.aisoldev.bizmap.data.api

import okhttp3.Interceptor
import okhttp3.Response

class AuthInterceptor : Interceptor {

    @Volatile
    var token: String? = null

    override fun intercept(chain: Interceptor.Chain): Response {
        val request = chain.request()

        val authenticatedRequest = token?.let {
            request.newBuilder()
                .header("Authorization", "Bearer $it")
                .build()
        } ?: request

        return chain.proceed(authenticatedRequest)
    }
}
