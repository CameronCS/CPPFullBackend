add_library(Entities INTERFACE)

target_include_directories(Entities INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(Entities INTERFACE cxx_std_20)