#include <Geode/modify/LevelInfoLayer.hpp>

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/GJDifficultySprite.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/loader/Mod.hpp>
#include <fmt/format.h>

#include "../horn/Manager.hpp"
#include "GJDifficultySprite.hpp"

class $modify(CustomLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        std::optional<horn::LevelInfo> m_levelInfo;
        CCMenuItemSpriteExtra* m_difficultyButton;
    };

    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1)) {
            return false;
        }

        auto cache = horn::Manager::sharedManager()->getCache();
        m_fields->m_levelInfo = cache.getLevelInfo(level->m_levelID);

        if (m_fields->m_levelInfo) {
            int tier = m_fields->m_levelInfo->tier();

            addHintArt();
            addTierText(tier);

            addButton();
            updateButton();

            static_cast<CustomGJDifficultySprite*>(m_difficultySprite)->decorateFromTier(tier);
        }

        return true;
    }

    void updateLabelValues() {
        LevelInfoLayer::updateLabelValues();

        if (m_fields->m_levelInfo) {
            updateButton();
        }
    }

    //! @brief Add hint art if not already shown.
    void addHintArt() {
        if (horn::Manager::sharedManager()->getHintShown()) {
            return;
        }

        cocos2d::CCPoint position = {
            m_difficultySprite->getPositionX() - 47.0f,
            m_difficultySprite->getPositionY() + 12.0f
        };

        auto* hint = cocos2d::CCSprite::createWithSpriteFrameName(
            "hint_001.png"_spr
        );
        hint->setPosition(position);
        hint->setID("und-button-hint"_spr);

        addChild(hint);
    }

    //! @brief Add tier text.
    void addTierText(int tier) {
        if (!horn::Manager::sharedManager()->showTierText()) {
            return;
        }

        cocos2d::CCPoint position = m_difficultySprite->getPosition();
        position.y -= 43.0f;
        if (m_coins->count()) {
            position.y -= 16.0f;
        }
        if (m_level->m_gauntletLevel || m_level->m_gauntletLevel2) {
            position.y -= 14.5f;
        }

        std::string text = fmt::format("UND T{}", tier);
        auto* tierSprite = cocos2d::CCLabelBMFont::create(text.c_str(), "bigFont.fnt", 100.0f);
        tierSprite->setPosition(position);
        tierSprite->setScale(0.333f);
        static cocos2d::ccColor3B colors[6] = {
            { 0x60, 0xff, 0xfe },
            { 0x5f, 0xfb, 0x3a },
            { 0xff, 0xfc, 0x42 },
            { 0xf6, 0x97, 0x2c },
            { 0xf2, 0x0b, 0x1d },
            { 0x00, 0x00, 0x00 }
        };
        if (tier >= 0 && tier <= 6) {
            tierSprite->setColor(colors[tier - 1]);
        }
        tierSprite->setID("und-tier-text"_spr);

        addChild(tierSprite);
    }

    //! @brief Add difficulty button.
    //! @param levelInfo Level info.
    void addButton() {
        auto* menu = cocos2d::CCMenu::create();
        menu->ignoreAnchorPointForPosition(false);
        menu->setContentSize({ 80.0f, 80.0f });
        menu->setPosition(m_difficultySprite->getPosition());

        m_fields->m_difficultyButton = CCMenuItemSpriteExtra::create(
            m_difficultySprite,
            this,
            menu_selector(CustomLevelInfoLayer::onDifficulty)
        );
        m_fields->m_difficultyButton->setPosition({
            menu->getContentSize() / 2.0f
        });

        menu->addChild(m_fields->m_difficultyButton);
        menu->setID("und-difficulty-button"_spr);
        addChild(menu);

        removeChild(m_difficultySprite);
    }

    //! @brief Unfuck difficulty button position.
    void updateButton() {
        m_difficultySprite->setPosition({
            m_fields->m_difficultyButton->getContentSize() / 2.0f
        });
    }

    //! @brief Difficulty button clicked callback.
    void onDifficulty(cocos2d::CCObject* sender) {
        horn::Manager::sharedManager()->setHintShown(true);

        std::string body = fmt::format(
            "<cr>Tier</c>: {}" "\n"
            "<cl>Skillset</c>: {}" "\n"
            "\n"
            "{}",
            m_fields->m_levelInfo->tier(),
            m_fields->m_levelInfo->skillset(),
            m_fields->m_levelInfo->description()
        );

        FLAlertLayer::create(
            nullptr,
            m_level->m_levelName.c_str(),
            body,
            "OK",
            nullptr,
            400.0f
        )->show();
    }
};
