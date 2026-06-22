#ifndef _____DEPENDENCY_INJECTION_CONFIG_H_____
#define _____DEPENDENCY_INJECTION_CONFIG_H_____

#include <DependencyInjection/DependencyContainer.h>
#include <DependencyInjection/ScopedContainer.h>
#include <DataService.h>
#include <BusinessService.h>
#include <Logging/Logger.h>
#include <Mapping/Mapping.h>

namespace DependencyConfig {
	using Logger = SystemFramework::Logging::Logger;
	using Mapper = SystemFramework::Mapping::Mapper;

	struct DependencyRequirements {
		std::string ConnectionString = "";
		Logger* BusinessServiceLogger = 0;
		Logger* DataServiceLogger = 0;
		Mapper* Mapper = 0;
	};

	void RegisterDependencies(DependencyConfig::DependencyRequirements requirements, SystemFramework::DependencyInjection::DependencyContainer* container);
}
#endif