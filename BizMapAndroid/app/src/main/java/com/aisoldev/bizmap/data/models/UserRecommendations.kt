package com.aisoldev.bizmap.data.models

data class UserRecommendations(
    val popularSearches: List<String> = emptyList(),
    val popularCategories: List<String> = emptyList()
)
