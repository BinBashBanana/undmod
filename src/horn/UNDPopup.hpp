#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/utils/cocos.hpp>

#include "UNDPopupBody.hpp"

namespace horn {

//! @brief Popup modal window for UNDs.
class UNDPopup : public geode::Popup {
public:
    static UNDPopup* create() {
        auto ret = new UNDPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

private:
    bool init() override;
    void onEnter() override;
    void onExit() override;
    void clickTabAll(cocos2d::CCObject* sender);
    void clickTabPacks(cocos2d::CCObject* sender);
    void clickTabRandomizer(cocos2d::CCObject* sender);

    ButtonSprite* m_btnSpriteTabAll;
    ButtonSprite* m_btnSpriteTabPacks;
    ButtonSprite* m_btnSpriteTabRandomizer;
    CCMenuItemSpriteExtra* m_btnTabAll;
    CCMenuItemSpriteExtra* m_btnTabPacks;
    CCMenuItemSpriteExtra* m_btnTabRandomizer;

    cocos2d::CCNode* m_bodyContainer;
    UNDPopupBody* m_body = nullptr;
    UNDPopupTab m_activeTab = UNDPopupTab::AllLevels;
    UNDPopupBodyState m_bodyState{};

    std::unordered_map<int, geode::Ref<GJGameLevel>> m_gameLevelCache;
};

}
