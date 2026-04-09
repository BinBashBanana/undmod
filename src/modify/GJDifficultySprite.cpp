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

void CustomGJDifficultySprite::updateFeatureState(GJFeatureState state) {
    GJDifficultySprite::updateFeatureState(state);
    if (currentLevelCellId >= 0) {
        auto cache = horn::Manager::sharedManager()->getCache();
        auto levelInfo = cache.getLevelInfo(currentLevelCellId);
        if (!levelInfo) {
            return;
        }
        decorateFromTier(levelInfo->tier());
    }
}

//! @brief Add horns/eyes to difficulty sprite if enabled.
void CustomGJDifficultySprite::decorateFromTier(int tier) {
    if (horn::Manager::sharedManager()->showHorns()) {
        updateHorns(tier);
        if (tier == 6) {
            updateEyes();
        }
    }
}

//! @brief Add horns.
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

//! @brief Add eyes for tier 6.
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
