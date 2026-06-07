#include "GJDifficultySprite.hpp"

#include <fmt/format.h>

#include "../horn/Manager.hpp"

// This seems hacky, but I think it should always work.
static int currentLevelCellId = -1;
namespace horn {
    void setLoadingLevelCellId(int id) {
        currentLevelCellId = id;
    }
}

static const std::unordered_map<int, bool> easterEggLevels = {
    { 13518208, false },
    { 14031694, false },
    { 21354428, true }
};

void CustomGJDifficultySprite::updateFeatureState(GJFeatureState state) {
    GJDifficultySprite::updateFeatureState(state);
    if (currentLevelCellId >= 0) {
        const auto& cache = horn::Manager::sharedManager()->getCache();
        auto levelInfo = cache.getLevelInfo(currentLevelCellId);
        if (!levelInfo) {
            return;
        }
        decorate(levelInfo->tier(), currentLevelCellId);
    }
}

void CustomGJDifficultySprite::decorate(int tier, int levelID) {
    if (horn::Manager::sharedManager()->showHorns()) {
        auto easterEgg = easterEggLevels.find(levelID);
        if (easterEgg != easterEggLevels.end()) {
            updateEasterEgg(levelID);
            if (easterEgg->second) {
                return;
            }
        }

        updateHorns(tier);
        if (tier == 6) {
            updateEyes();
        }
    }
}

void CustomGJDifficultySprite::updateEasterEgg(int levelID) {
    if (m_fields->m_easterEggCreated) {
        return;
    }

    std::string frame = fmt::format("{}.png"_spr, levelID);
    auto* newFace = cocos2d::CCSprite::createWithSpriteFrameName(
        frame.c_str()
    );
    newFace->setPosition(getContentSize() / 2.0f);
    addChild(newFace);
    setOpacity(0);

    m_fields->m_easterEggCreated = true;
}

void CustomGJDifficultySprite::updateHorns(int tier) {
    if (m_fields->m_hornsCreated) {
        return;
    }

    std::string frame = fmt::format("horn{:02}_001.png"_spr, tier);
    auto* horns = cocos2d::CCSprite::createWithSpriteFrameName(
        frame.c_str()
    );
    horns->setPosition(getContentSize() / 2.0f);
    addChild(horns, -1);

    m_fields->m_hornsCreated = true;
}

void CustomGJDifficultySprite::updateEyes() {
    if (m_fields->m_eyesCreated) {
        return;
    }

    auto* eyes = cocos2d::CCSprite::createWithSpriteFrameName(
        "eye06_001.png"_spr
    );
    eyes->setPosition(getContentSize() / 2.0f);
    eyes->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
    addChild(eyes, 2);

    m_fields->m_eyesCreated = true;
}
