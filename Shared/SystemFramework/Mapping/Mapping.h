#ifndef _____MAPPER_H_____
#define _____MAPPER_H_____

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <stdexcept>
#include <memory>

namespace SystemFramework::Mapping {
	class Mapper {
	public:
		template<typename TSource, typename TDest> void Register(std::function<TDest(const TSource&)> mappingFunc) {
			std::pair<std::type_index, std::type_index> key = std::make_pair(std::type_index(typeid(TSource)), std::type_index(typeid(TDest)));
			_mappings[key] =
				[mappingFunc](const void* src) -> void*
				{
					return new TDest(mappingFunc(*static_cast<const TSource*>(src)));
				};
		}

		template<typename TSource, typename TDest> TDest Map(const TSource& source) {
			std::pair<std::type_index, std::type_index> key = std::make_pair(std::type_index(typeid(TSource)), std::type_index(typeid(TDest)));
			std::unordered_map<std::pair<std::type_index, std::type_index>, std::function<void*(const void*)>>::iterator it = _mappings.find(key);
			if (it == _mappings.end()) {
				throw std::runtime_error("No mapping registered for these types");
			}
			std::unique_ptr<TDest> result(static_cast<TDest*>(it->second(&source)));
			return std::move(*result);
		}

	private:
		struct PairHash {
			std::size_t operator()(const std::pair<std::type_index, std::type_index>& pair) const {
				std::size_t h1 = std::hash<std::type_index>{}(pair.first);
				std::size_t h2 = std::hash<std::type_index>{}(pair.second);
				return h1 ^ (h2 << 1);
			}
		};
		std::unordered_map<std::pair<std::type_index, std::type_index>, std::function<void*(const void*)>, PairHash> _mappings;
	};
}

#endif