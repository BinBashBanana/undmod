#include <Geode/modify/LevelCell.hpp>
#include <Geode/binding/GJGameLevel.hpp>

#include "GJDifficultySprite.hpp"

class $modify(LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        horn::setLoadingLevelCellId(level->m_levelID);
        LevelCell::loadFromLevel(level);
        horn::setLoadingLevelCellId(-1);
    }
};
