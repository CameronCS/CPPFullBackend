#pragma once

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
		Logger* BusinessServiceLogger = nullptr;
		Logger* DataServiceLogger = nullptr;
		Mapper* MapperInstance = nullptr;
	};

	void RegisterDependencies(DependencyConfig::DependencyRequirements requirements, SystemFramework::DependencyInjection::DependencyContainer* container);
}