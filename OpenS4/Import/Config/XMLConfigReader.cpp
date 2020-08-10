
#include "XMLConfigReader.hpp"

#include <tinyxml2/tinyxml2.h>

#include <Logger/Logger.hpp>
#include <exception>

namespace OpenS4::Import::Config
{
    Goods stringToGood(std::string string)
    {
        if (string == "GOOD_NO_GOOD") return Goods::NO_GOOD;
        if (string == "GOOD_AGAVE") return Goods::AGAVE;
        if (string == "GOOD_AMMO") return Goods::AMMO;
        if (string == "GOOD_ARMOR") return Goods::ARMOR;
        if (string == "GOOD_AXE") return Goods::AXE;
        if (string == "GOOD_BATTLEAXE") return Goods::BATTLEAXE;
        if (string == "GOOD_BLOWGUN") return Goods::BLOWGUN;
        if (string == "GOOD_BOARD") return Goods::BOARD;
        if (string == "GOOD_BOW") return Goods::BOW;
        if (string == "GOOD_BREAD") return Goods::BREAD;
        if (string == "GOOD_COAL") return Goods::COAL;
        if (string == "GOOD_FISH") return Goods::FISH;
        if (string == "GOOD_FLOUR") return Goods::FLOUR;
        if (string == "GOOD_GOAT") return Goods::GOAT;
        if (string == "GOOD_GOLDBAR") return Goods::GOLDBAR;
        if (string == "GOOD_GOLDORE") return Goods::GOLDORE;
        if (string == "GOOD_GRAIN") return Goods::GRAIN;
        if (string == "GOOD_GUNPOWDER") return Goods::GUNPOWDER;
        if (string == "GOOD_HAMMER") return Goods::HAMMER;
        if (string == "GOOD_HONEY") return Goods::HONEY;
        if (string == "GOOD_IRONBAR") return Goods::IRONBAR;
        if (string == "GOOD_IRONORE") return Goods::IRONORE;
        if (string == "GOOD_LOG") return Goods::LOG;
        if (string == "GOOD_MEAD") return Goods::MEAD;
        if (string == "GOOD_MEAT") return Goods::MEAT;
        if (string == "GOOD_PICKAXE") return Goods::PICKAXE;
        if (string == "GOOD_PIG") return Goods::PIG;
        if (string == "GOOD_ROD") return Goods::ROD;
        if (string == "GOOD_SAW") return Goods::SAW;
        if (string == "GOOD_SCYTHE") return Goods::SCYTHE;
        if (string == "GOOD_SHEEP") return Goods::SHEEP;
        if (string == "GOOD_SHOVEL") return Goods::SHOVEL;
        if (string == "GOOD_STONE") return Goods::STONE;
        if (string == "GOOD_SULFUR") return Goods::SULFUR;
        if (string == "GOOD_SWORD") return Goods::SWORD;
        if (string == "GOOD_TEQUILA") return Goods::TEQUILA;
        if (string == "GOOD_WATER") return Goods::WATER;
        if (string == "GOOD_WINE") return Goods::WINE;
        if (string == "GOOD_BACKPACKCATAPULT") return Goods::BACKPACKCATAPULT;
        if (string == "GOOD_GOOSE") return Goods::GOOSE;
        if (string == "GOOD_EXPLOSIVEARROW") return Goods::EXPLOSIVEARROW;
        if (string == "GOOD_SUNFLOWEROIL") return Goods::SUNFLOWEROIL;
        if (string == "GOOD_SUNFLOWER") return Goods::SUNFLOWER;

        OpenS4::getLogger().err("Unknown good: '%s'.", string.c_str());
        throw std::exception();
    }

    Race stringToRace(std::string string)
    {
        if (string == "RACE_ROMAN") return Race::ROMAN;
        if (string == "RACE_VIKING") return Race::VIKING;
        if (string == "RACE_MAYA") return Race::MAYA;
        if (string == "RACE_DARK") return Race::DARK;
        if (string == "RACE_TROJAN") return Race::TROJAN;

        OpenS4::getLogger().err("Unknown race: '%s'.", string.c_str());
        throw std::exception();
    }

    std::vector<ObjectDefinition> readObjectDefinitions(const char* data)
    {
        std::vector<ObjectDefinition> objectDefinitions;

        tinyxml2::XMLDocument doc;

        if (doc.Parse(data) != tinyxml2::XML_SUCCESS)
        {
            OpenS4::getLogger().err("Can't read XML data!");
            throw std::exception();
        }

        tinyxml2::XMLElement* objectlist = doc.FirstChildElement("objectlist");

        tinyxml2::XMLElement* object = objectlist->FirstChildElement("object");

        while (object)
        {
            ObjectDefinition objectDefinition;

            objectDefinition.name = object->FindAttribute("id")->Value();
            objectDefinition.blocking =
                std::stoi(object->FirstChildElement("blocking")->GetText());

            if (object->FirstChildElement("repellent")->GetText())
                objectDefinition.repellant =
                    std::stoi(
                        object->FirstChildElement("repellent")->GetText()) > 0;
            else
                objectDefinition.repellant =
                    0;  // TODO special value if value is not present?

            object = object->NextSiblingElement("object");

            objectDefinitions.push_back(objectDefinition);
        }

        return objectDefinitions;
    }

    std::vector<BuildingDefinition> readBuildingDefinitions(const char* data)
    {
        std::vector<BuildingDefinition> buildingDefinitions;

        tinyxml2::XMLDocument doc;

        if (doc.Parse(data) != tinyxml2::XML_SUCCESS)
        {
            OpenS4::getLogger().err("Can't read XML data!");
            throw std::exception();
        }

        tinyxml2::XMLElement* buildinglist =
            doc.FirstChildElement("buildinglist");

        tinyxml2::XMLElement* race = buildinglist->FirstChildElement("race");

        auto elementToInt = [](tinyxml2::XMLElement* element, std::string key) {
            return std::stoi(
                element->FirstChildElement(key.c_str())->GetText());
        };
        auto fillPosLines = [&elementToInt](std::vector<std::bitset<32>>& lines,
                                            tinyxml2::XMLElement* posLines) {
            tinyxml2::XMLElement* value = posLines->FirstChildElement("value");

            while (value)
            {
                int val = std::stoi(value->GetText());

                u32 val32 = (val)&0xffffffff;
                std::bitset<32> bits(val32);
                lines.push_back(val32);

                value = value->NextSiblingElement("value");
            }
        };

        while (race)
        {
            tinyxml2::XMLElement* building =
                race->FirstChildElement("building");

            Race r = stringToRace(race->FindAttribute("id")->Value());

            while (building)
            {
                BuildingDefinition bd;

                bd.race = r;
                bd.name = building->FindAttribute("id")->Value();
                bd.hotspotX = elementToInt(building, "iHotSpotX");
                bd.hotspotY = elementToInt(building, "iHotSpotY");

                bd.costStone = elementToInt(building, "stone");
                bd.costBoard = elementToInt(building, "boards");
                bd.costGold = elementToInt(building, "gold");

                fillPosLines(bd.buildingPosLines,
                             building->FirstChildElement("buildingPosLines"));
                fillPosLines(bd.digPosLines,
                             building->FirstChildElement("digPosLines"));
                fillPosLines(bd.repealingPosLines,
                             building->FirstChildElement("repealingPosLines"));
                fillPosLines(bd.blockPosLines,
                             building->FirstChildElement("blockPosLines"));
                fillPosLines(bd.waterPosLines,
                             building->FirstChildElement("waterPosLines"));

                if (building->FirstChildElement("waterFreePosLines"))
                    fillPosLines(bd.waterFreePosLines,
                                building->FirstChildElement("waterFreePosLines"));

                if (building->FirstChildElement("waterBlockPosLines"))
                fillPosLines(bd.waterBlockPosLines,
                             building->FirstChildElement("waterBlockPosLines"));

                auto bitBoundingRect =
                    building->FirstChildElement("bitBoundingRect");

                bd.boundingRect.xMin = elementToInt(bitBoundingRect, "minX");
                bd.boundingRect.yMin = elementToInt(bitBoundingRect, "minY");
                bd.boundingRect.xMax = elementToInt(bitBoundingRect, "maxX");
                bd.boundingRect.yMax = elementToInt(bitBoundingRect, "maxY");

                auto miniflag = building->FirstChildElement("miniflag");

                if (miniflag)
                {
                    bd.miniflagX = elementToInt(miniflag, "xOffset");
                    bd.miniflagY = elementToInt(miniflag, "yOffset");
                }
                else
                {
                    bd.miniflagX = -1; // TODO this should not happen? maybe the configs are wrong...
                    bd.miniflagY = -1;
                }

                auto pile = building->FirstChildElement("pile");

                while (pile)
                {
                    BuildingDefinition::Pile p;

                    p.good = stringToGood(
                        pile->FirstChildElement("good")->GetText());

                    p.xPixelOffset = elementToInt(pile, "xPixelOffset");
                    p.yPixelOffset = elementToInt(pile, "yPixelOffset");

                    p.xOffset = elementToInt(pile, "xOffset");
                    p.yOffset = elementToInt(pile, "yOffset");

                    p.produced = elementToInt(pile, "type") == 0;

                    bd.piles.push_back(p);

                    pile = pile->NextSiblingElement("pile");
                }

                auto builder = building->FirstChildElement("builderInfo");

                while (builder)
                {
                    BuildingDefinition::Builder b;

                    b.xOffset = elementToInt(builder, "xOffset");
                    b.yOffset = elementToInt(builder, "yOffset");
                    b.direction = elementToInt(builder, "dir");

                    bd.builderPositions.push_back(b);

                    builder = builder->NextSiblingElement("builder");
                }

                buildingDefinitions.push_back(bd);

                building = building->NextSiblingElement("building");
            }

            race = race->NextSiblingElement("race");
        }

        return buildingDefinitions;
    }

}  // namespace OpenS4::Import::Config
