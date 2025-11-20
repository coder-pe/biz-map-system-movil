package com.aisoldev.bizmap.data.models

data class RegisterRequest(
    val username: String,
    val email: String,
    val password: String,
    val fullName: String,
    val phone: String = ""
)
