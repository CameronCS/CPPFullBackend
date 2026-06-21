add_library(BusinessService STATIC
    "ProductService/ProductService.cpp"
    "ProductService/ProductService.h"
    "BusinessService.h"
)

target_include_directories(BusinessService PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Interfaces/BusinessServiceInterface
    ${CMAKE_SOURCE_DIR}/Interfaces/DataServiceInterface
    ${CMAKE_SOURCE_DIR}/Shared/Models
    ${CMAKE_SOURCE_DIR}/Shared/Entities
)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(BusinessService PRIVATE nlohmann_json::nlohmann_json Entities Models BusinessServiceInterface DataServiceInterface)

target_compile_features(BusinessService PRIVATE cxx_std_20)