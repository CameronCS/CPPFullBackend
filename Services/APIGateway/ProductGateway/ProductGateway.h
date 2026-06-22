#pragma once

#include <iostream>
#include <httplib.h>
#include <IProductService.h>
#include <Logging/ILogger.h>
#include <Gateway/GatewayBase.h>
#include <DependencyInjection/DependencyContainer.h>

namespace APIGateway {
	class ProductGateway : public SystemFramework::Gateway::GatewayBase {
	public:
		ProductGateway(httplib::SSLServer& server, SystemFramework::DependencyInjection::DependencyContainer* container, SystemFramework::Logging::ILogger* logger) {
			this->_container = container;
			this->_logger = logger;
			logger->SetServiceLevel("API", "Product Gateway");
			MapMethods(server);
		}

	private:
		SystemFramework::DependencyInjection::DependencyContainer* _container;
		SystemFramework::Logging::ILogger* _logger;
		void MapMethods(httplib::SSLServer& server);
		void AddProduct(const httplib::Request& req, httplib::Response& res);
		void GetAllProducts(const httplib::Request& req, httplib::Response& res);
		void GetProductById(const httplib::Request& req, httplib::Response& res);
		void DeleteProduct(const httplib::Request& req, httplib::Response& res);
		void UpdateProduct(const httplib::Request& req, httplib::Response& res);
	};
}