package com.aisoldev.bizmap.data.models

data class Business(
    val id: String,
    val ownerId: String,
    val name: String,
    val description: String = "",
    val category: String = "",
    val phone: String = "",
    val email: String = "",
    val website: String = "",
    val address: String = "",
    val location: GeoLocation = GeoLocation(),
    val rating: Double = 0.0,
    val totalReviews: Int = 0,
    val isVerified: Boolean = false,
    val isActive: Boolean = true,
    val images: List<String> = emptyList(),
    val createdAt: Long = 0,
    val updatedAt: Long = 0,
    val distanceMeters: Double? = null
)
