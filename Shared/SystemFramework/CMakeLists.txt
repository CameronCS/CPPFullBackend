add_library(SystemFramework STATIC
    "SystemFramework.h"
    "Logging/Logging.h"
    "Logging/ILogger.h" 
    "Logging/LoggerColours.h" 
    "Logging/Logger.h" 
    "Logging/Logger.cpp"
 "Gateway/GatewayBase.h"
    "DependencyInjection/ScopedContainer.h"
    "DependencyInjection/DependencyContainer.h"
)

 find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(SystemFramework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SystemFramework PRIVATE nlohmann_json::nlohmann_json )

target_compile_features(SystemFramework PRIVATE cxx_std_20)