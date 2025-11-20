package com.aisoldev.bizmap.data.models

data class ProductWithBusiness(
    val product: Product,
    val business: Business,
    val distanceMeters: Double? = null
)
