package com.aisoldev.bizmap.data.models

data class ApiError(
    val statusCode: Int = 0,
    val errorMessage: String = "",
    val errorCode: String = ""
)
