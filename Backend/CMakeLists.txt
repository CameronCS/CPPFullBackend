# CMakeList.txt : CMake project for CMakebackend, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Backend "Main.cpp" "Main.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Backend PROPERTY CXX_STANDARD 20)
endif()
find_package(httplib CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(nanodbc CONFIG REQUIRED)

target_link_libraries(Backend
    PRIVATE
    APIService
    BusinessService
    DataService

    httplib::httplib
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    nanodbc
)

configure_file(appsettings.json appsettings.json COPYONLY)

# TODO: Add tests and install targets if needed.
add_custom_command(TARGET Backend POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:Backend>
        $<TARGET_FILE_DIR:Backend>
    COMMAND_EXPAND_LISTS
)
# add_custom_command(TARGET Backend POST_BUILD ...)
# 
# Run a custom command attached to the Backend target, specifically after it finishes building. You could also use PRE_BUILD or PRE_LINK.
# ${CMAKE_COMMAND}
# 
# This is just the path to the cmake.exe that's currently running. Using it instead of hardcoding cmake.exe makes it portable.
# -E copy_if_different
# 
# The -E flag puts CMake into "command mode" where it runs built-in utilities. copy_if_different is one of them — it copies a file but only if it's actually changed, so it doesn't unnecessarily touch files on every build.
# $<TARGET_RUNTIME_DLLS:Backend>
# 
# This is a generator expression — those $<...> things. This one expands to the list of all DLLs your target needs at runtime. CMake figures this out automatically from what you linked.
# $<TARGET_FILE_DIR:Backend>
# 
# Another generator expression — expands to the directory where your .exe was output. So the DLLs end up right next to it.
# COMMAND_EXPAND_LISTS
# 
# The DLL list comes out as a.dll;b.dll;c.dll — a semicolon separated string. Without this flag CMake passes it as one argument and the copy fails. This flag tells CMake to split it into separate arguments first.