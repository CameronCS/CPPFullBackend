#include "MapperConfig.h"

void MapperConfig::MapperConfig(SystemFramework::Mapping::Mapper* mapper) {
	mapper->Register<Models::Product, Entities::PRDProduct>(
		[](const Models::Product& src) {
			return Entities::PRDProduct(src.Id, src.Name, src.Price);
		}
	);

	mapper->Register<Entities::PRDProduct, Models::Product>(
		[](const Entities::PRDProduct& src) {
			return Models::Product(src.ID, src.Name, src.Price);
		}
	);
}