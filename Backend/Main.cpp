#include "Main.h"

namespace {
	nlohmann::json LoadConfig(const std::string& path) {
		std::ifstream file(path);
		if (!file.is_open()) {
			throw std::runtime_error("Could not open config file: " + path);
		}
		return nlohmann::json::parse(file);
	}
}

int main() {
	InitConsole(); // Only here for WIN32 Has Guard in header so no guard needed here
	nlohmann::json config = LoadConfig("appsettings.json");

	std::string connStr = config["database"]["connectionString"];

	std::string host = config["server"]["host"];
	int port = config["server"]["port"];
	std::string certPath = config["server"]["certPath"];
	std::string keyPath = config["server"]["keyPath"];

	SystemFramework::Logging::Logger dataLogger;
	SystemFramework::Logging::Logger businessLogger;
	SystemFramework::Logging::Logger apiLogger;

	SystemFramework::Mapping::Mapper mapper;
	MapperConfig::MapperConfig(&mapper);

	DependencyConfig::DependencyRequirements dependencyRequirements;
	dependencyRequirements.ConnectionString = connStr;
	dependencyRequirements.BusinessServiceLogger = &businessLogger;
	dependencyRequirements.DataServiceLogger = &dataLogger;
	dependencyRequirements.MapperInstance = &mapper;
	SystemFramework::DependencyInjection::DependencyContainer container;
	DependencyConfig::RegisterDependencies(dependencyRequirements, &container);

	httplib::SSLServer server(certPath.c_str(), keyPath.c_str());

	APIGateway::ProductGateway productgateway(server, &container, &apiLogger);

	server.Get("/system/heartbeat", [connStr](const httplib::Request&, httplib::Response& res) {
		try {
			nanodbc::connection conn(connStr);
			nanodbc::execute(conn, NANODBC_TEXT("SELECT 1"));
			res.set_content("{\"status\": \"Ok\", \"database\": \"Connected\"}", "application/json");
			res.status = httplib::OK_200;
		}
		catch (const std::exception&) {
			res.set_content("{\"status\": \"Degraded\", \"database\": \"Disconnected\"}", "application/json");
			res.status = httplib::ServiceUnavailable_503;
		}
	});

	std::cout << "Server running on https://localhost:" << port << std::endl;
	std::cout << "Server Heartbeat https://localhost:" << port << "/system/heartbeat" << std::endl;
	server.listen(host, port);

	return 0;
}