add_library(BusinessService STATIC
    "ProductService.cpp"
    "ProductService.h"
)

target_include_directories(BusinessService PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(BusinessService PRIVATE DataService nlohmann_json::nlohmann_json Entities Models BusinessServiceInterface DataServiceInterface)

target_compile_features(BusinessService PRIVATE cxx_std_20)