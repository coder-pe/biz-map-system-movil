# Examples for BizMap C++ Library

# Basic C++ API example
add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example PRIVATE BizMap::bizmap)

# C API example (for JNI/Swift integration reference)
add_executable(c_api_example c_api_example.cpp)
target_link_libraries(c_api_example PRIVATE BizMap::bizmap)

# Set C++ standard for examples
set_target_properties(basic_example c_api_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Install examples (optional)
install(TARGETS basic_example c_api_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)
