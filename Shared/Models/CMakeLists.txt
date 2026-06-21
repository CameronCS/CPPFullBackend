add_library(Models INTERFACE)

target_include_directories(Models INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(Models INTERFACE nlohmann_json::nlohmann_json)

target_compile_features(Models INTERFACE cxx_std_20)