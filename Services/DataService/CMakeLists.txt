add_library(DataService STATIC
    "ProductRepository.cpp"
    "ProductRepository.h"
 "DBUtils/DBUtils.h" "DBUtils/DBUtils.cpp")

target_include_directories(DataService PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Interfaces/DataServiceInterface
    ${CMAKE_SOURCE_DIR}/Shared/Entities
)

find_package(nanodbc CONFIG REQUIRED)

target_link_libraries(DataService PRIVATE nanodbc Entities DataServiceInterface)

target_compile_features(DataService PRIVATE cxx_std_20)