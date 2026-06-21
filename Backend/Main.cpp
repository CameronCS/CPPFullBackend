// Main.cpp : Defines the entry point for the application.
//

#include "Main.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <nanodbc/nanodbc.h>
#include <ProductGateway.h>
#include <ProductService.h>
#include <ProductRepository.h>

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
    nlohmann::json config = LoadConfig("appsettings.json");

    std::string connStr = config["database"]["connectionString"];
    std::string host = config["server"]["host"];
    int port = config["server"]["port"];

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

    httplib::Server svr;

    DataService::ProductRepository productRepo;
    BusinessService::ProductService productService(&productRepo);
    APIGateway::ProductGateway gateway(svr, &productService);

    svr.Get("/system/heartbeat", Heartbeat);

    std::cout << "Server running on http://localhost:" << port << std::endl;
    std::cout << "Server Heartbeat http://localhost:" << port << "/system/heartbeat" << std::endl;
    svr.listen(host, port);

    return 0;
}