add_library(APIService STATIC
    "ProductGateway.cpp"
    "ProductGateway.h"
)

find_package(httplib CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(
    APIService 
        PRIVATE 
        httplib::httplib 
        OpenSSL::SSL 
        OpenSSL::Crypto 
        nlohmann_json::nlohmann_json 
        Models 
        BusinessServiceInterface
)

target_include_directories(APIService PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(APIService PRIVATE cxx_std_20)