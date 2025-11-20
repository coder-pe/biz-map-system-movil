package com.aisoldev.bizmap.data.models

data class Product(
    val id: String,
    val businessId: String,
    val name: String,
    val description: String = "",
    val price: Double,
    val currency: String = "PEN",
    val category: String = "",
    val isAvailable: Boolean = true,
    val stockQuantity: Int = 0,
    val images: List<String> = emptyList(),
    val createdAt: Long = 0,
    val updatedAt: Long = 0
)
