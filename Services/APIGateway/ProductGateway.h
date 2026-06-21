#ifndef _____PRODUCT_GATEWAY_H_____
#define _____PRODUCT_GATEWAY_H_____

#include <iostream>
#include <httplib.h>
#include <IProductService.h>

namespace APIGateway {
	class ProductGateway {
	public:
		ProductGateway(httplib::SSLServer& server, BusinessService::Interface::IProductService* productService) {
			this->_productService = productService;
			MapMethods(server);
		}

	private:
		BusinessService::Interface::IProductService* _productService;
		void MapMethods(httplib::SSLServer& server);
		void AddProduct(const httplib::Request& req, httplib::Response& res);
		void GetAllProducts(const httplib::Request& req, httplib::Response& res);
		void GetProductById(const httplib::Request& req, httplib::Response& res);
		void DeleteProduct(const httplib::Request& req, httplib::Response& res);
		void UpdateProduct(const httplib::Request& req, httplib::Response& res);
	};
}

#endif