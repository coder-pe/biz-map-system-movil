package com.aisoldev.bizmap.data.models

data class GeoLocation(
    val latitude: Double = 0.0,
    val longitude: Double = 0.0
) {
    fun isValid(): Boolean =
        latitude in -90.0..90.0 && longitude in -180.0..180.0
}
