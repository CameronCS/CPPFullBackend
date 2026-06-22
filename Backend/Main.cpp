// Main.cpp : Defines the entry point for the application.
//

#include "Main.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <nanodbc/nanodbc.h>
#include <APIGateway.h>
#include <BusinessService.h>
#include <DataService.h>
#include <SystemFramework.h>
#if WIN32
#include <Windows.h>
#endif

static std::string connectionString;

static nlohmann::json LoadConfig(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + path);
	}
	return nlohmann::json::parse(file);
}

static void Heartbeat(const httplib::Request& req, httplib::Response& res) {
	try {
		nanodbc::connection conn(connectionString);
		nanodbc::execute(conn, NANODBC_TEXT("SELECT 1"));
		res.set_content("{\"status\": \"Ok\", \"database\": \"Connected\"}", "application/json");
		res.status = httplib::OK_200;
	}
	catch (const std::exception& e) {
		res.set_content("{\"status\": \"Degraded\", \"database\": \"Disconnected\"}", "application/json");
		res.status = httplib::ServiceUnavailable_503;
	}
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
	connectionString = connStr;
	std::string host = config["server"]["host"];
	int port = config["server"]["port"];
	std::string certPath = config["server"]["certPath"];
	std::string keyPath = config["server"]["keyPath"];

	SystemFramework::Logging::Logger dataLogger;
	SystemFramework::Logging::Logger businessLogger;
	SystemFramework::Logging::Logger apiLogger;

	SystemFramework::DependencyInjection::DependencyContainer container;

	SystemFramework::Mapping::Mapper mapper;

	mapper.Register<Models::Product, Entities::PRDProduct>(
		[](const Models::Product& src) {
			return Entities::PRDProduct(src.Id, src.Name, src.Price);
		}
	);

	mapper.Register<Entities::PRDProduct, Models::Product>(
		[](const Entities::PRDProduct& src) {
			return Models::Product(src.ID, src.Name, src.Price);
		}
	);

	container.Register<DataService::Interface::IProductRepository>(
		[&connStr, &dataLogger](SystemFramework::DependencyInjection::ScopedContainer&) -> void* {
			return new DataService::ProductRepository(connStr, &dataLogger);
		}
	);

	container.Register<BusinessService::Interface::IProductService>(
		[&businessLogger, &mapper](SystemFramework::DependencyInjection::ScopedContainer& scope) -> void* {
			DataService::Interface::IProductRepository* repo = scope.Resolve<DataService::Interface::IProductRepository>();
			return new BusinessService::ProductService(repo, &businessLogger, &mapper);
		}
	);

	httplib::SSLServer server(certPath.c_str(), keyPath.c_str());

	APIGateway::ProductGateway productgateway(server, &container, &apiLogger);

	server.Get("/system/heartbeat", Heartbeat);

	std::cout << "Server running on https://localhost:" << port << std::endl;
	std::cout << "Server Heartbeat https://localhost:" << port << "/system/heartbeat" << std::endl;
	server.listen(host, port);

	return 0;
}