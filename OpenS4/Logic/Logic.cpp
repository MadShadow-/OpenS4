

#include "Logic.hpp"

#include "Map/Map.hpp"

namespace OpenS4::Logic {

RomanAnimations operator++(RomanAnimations& x) {
    return x = (RomanAnimations)(
               std::underlying_type<RomanAnimations>::type(x) + 1);
}

RomanAnimations operator*(RomanAnimations c) { return c; }

RomanAnimations begin(RomanAnimations r) { return RomanAnimations::DUMMY; }

RomanAnimations end(RomanAnimations r) {
    RomanAnimations l = RomanAnimations::_COUNT;
    return l;
}

EntityProperties* test_makeProperties(
    OpenS4::Import::GraphicsRegistry* registry) {
    EntityProperties* properties = new EntityProperties();
    /*
    Animation animation;
    animation.frameStart = 2970;
    animation.frameEnd = 3017;
    animation.duration = 10;
    //(animation.frameEnd - animation.frameStart) + 1;
    animation.name = "CARRIER_IDLE1";
    properties->animations.push_back(animation);
    */

    std::vector<u32> beginnings;
    u32 lastPal = 0;
    for (int i = 0; i < registry->getNumberOfImages(20); i++) {
        auto pal = registry->getJobIndex(20, i);
        if (pal != lastPal || lastPal == 0) {
            lastPal = pal;
            beginnings.push_back(i);
        }
    }

    for (auto roman : OpenS4::Logic::RomanAnimations()) {
        AnimationFrames frames;
        frames.frameStart = 0;
        frames.frameEnd = 0;

        if ((u32)roman < beginnings.size()) {
            frames.frameStart = beginnings[(u32)roman];
            if ((u32)(roman) + 1 < beginnings.size())
                frames.frameEnd = beginnings[(u32)roman + 1] - 1;
            else
                frames.frameEnd = registry->getNumberOfImages(20) - 1;
        }

        Animation animation;
        animation.frameStart = frames.frameStart;
        animation.frameEnd = frames.frameEnd;
        animation.duration = 2;
        animation.name = "";
        properties->animations.push_back(animation);
    }
    /*

    Animation animation;
    animation.frameStart = 3096;
    animation.frameEnd = 3269;
    animation.duration = 3;
    animation.name = "CARRIER_IDLE2";
    properties->animations.push_back(animation);

    */

    TaskList tasklist;

    properties->tasklists.push_back(tasklist);

    return properties;
}

}  // namespace OpenS4::Logic
