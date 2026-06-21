add_library(SystemFramework STATIC
    "SystemFramework.cpp"
    "SystemFramework.h"
)

target_include_directories(SystemFramework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(SystemFramework PRIVATE cxx_std_20)