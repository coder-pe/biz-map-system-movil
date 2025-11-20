package com.aisoldev.bizmap.data.models

import kotlinx.serialization.Serializable

@Serializable
data class User(
    val id: String,
    val username: String,
    val email: String,
    val fullName: String,
    val phone: String = "",
    val isActive: Boolean = true
)
