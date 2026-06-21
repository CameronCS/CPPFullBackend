// Main.cpp : Defines the entry point for the application.
//

#include "Main.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <nanodbc/nanodbc.h>
#include <APIGateway.h>
#include <BusinessService.h>
#include <DataService.h>
#include <Logging/Logging.h>
#if WIN32
#include <Windows.h>
#endif

static nlohmann::json LoadConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + path);
    }
    return nlohmann::json::parse(file);
}

static void Heartbeat(const httplib::Request& req, httplib::Response& res) {
    res.set_content("{\"Status\": \"Ok\"}", "application/json");
}

int main() {
#if WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    nlohmann::json config = LoadConfig("appsettings.json");

    std::string connStr = config["database"]["connectionString"];
    std::string host = config["server"]["host"];
    int port = config["server"]["port"];
    std::string certPath = config["server"]["certPath"];
    std::string keyPath = config["server"]["keyPath"];

    try {
        nanodbc::connection conn(connStr);
        std::cout << "Connected to database!" << std::endl;

        // quick test query
        nanodbc::result result = nanodbc::execute(conn, NANODBC_TEXT("SELECT @@VERSION"));
        while (result.next()) {
            std::cout << result.get<std::string>(0) << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "DB Error: " << e.what() << std::endl;
    }
    
    SystemFramework::Logging::Logger dataLogger;
    SystemFramework::Logging::Logger businessLogger;
    SystemFramework::Logging::Logger apiLogger;

    httplib::SSLServer server(certPath.c_str(), keyPath.c_str());

    DataService::ProductRepository productRepo(connStr, &dataLogger);
    BusinessService::ProductService productService(&productRepo, &businessLogger);
    APIGateway::ProductGateway productgateway(server, &productService, &apiLogger);

    server.Get("/system/heartbeat", Heartbeat);

    std::cout << "Server running on https://localhost:" << port << std::endl;
    std::cout << "Server Heartbeat https://localhost:" << port << "/system/heartbeat" << std::endl;
    server.listen(host, port);

    return 0;
}