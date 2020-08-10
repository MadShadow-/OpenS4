#pragma once


#include <Core/Types.hpp>

#include "Definitions.hpp"

#include <vector>

namespace OpenS4::Import::Config
{

	std::vector<ObjectDefinition> readObjectDefinitions(const char* data);


	std::vector<BuildingDefinition> readBuildingDefinitions(const char* data);


}  // namespace OpenS4::Import::Config
