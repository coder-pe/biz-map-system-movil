package com.aisoldev.bizmap.data.models

data class SearchHistoryEntry(
    val id: String,
    val userId: String,
    val query: String,
    val category: String = "",
    val location: GeoLocation = GeoLocation(),
    val createdAt: Long = 0
)
