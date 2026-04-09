#pragma once

#include <Geode/modify/GJDifficultySprite.hpp>

namespace horn {
    void setLoadingLevelCellId(int id);
}

class $modify(CustomGJDifficultySprite, GJDifficultySprite) {
public:
    struct Fields {
        bool m_hornsCreated = false;
        bool m_eyesCreated  = false;
    };

    void updateFeatureState(GJFeatureState state);

    //! @brief Add horns/eyes to difficulty sprite.
    void decorateFromTier(int tier);

private:
    //! @brief Add horns.
    void updateHorns(int tier);

    //! @brief Add eyes for tier 6.
    void updateEyes();
};
