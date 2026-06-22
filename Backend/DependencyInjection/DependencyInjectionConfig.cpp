#include "DependencyInjectionConfig.h"

void  DependencyConfig::RegisterDependencies(DependencyConfig::DependencyRequirements requirements, SystemFramework::DependencyInjection::DependencyContainer* container) {
	std::string connStr = requirements.ConnectionString;
	Logger* dataLogger = requirements.DataServiceLogger;
	Logger* businessLogger = requirements.BusinessServiceLogger;
	Mapper* mapper = requirements.Mapper;
	
	container->Register<DataService::Interface::IProductRepository>(
		[connStr, dataLogger](SystemFramework::DependencyInjection::ScopedContainer&) -> void* {
			return new DataService::ProductRepository(connStr, dataLogger);
		}
	);

	container->Register<BusinessService::Interface::IProductService>(
		[businessLogger, mapper](SystemFramework::DependencyInjection::ScopedContainer& scope) -> void* {
			DataService::Interface::IProductRepository* repo = scope.Resolve<DataService::Interface::IProductRepository>();
			return new BusinessService::ProductService(repo, businessLogger, mapper);
		}
	);
}