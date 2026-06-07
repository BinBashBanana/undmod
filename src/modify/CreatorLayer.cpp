#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

#include "../horn/Manager.hpp"
#include "../horn/UNDPopup.hpp"

class $modify(CustomCreatorLayer, CreatorLayer) {
    void onClick(cocos2d::CCObject* sender) {
        auto* popup = horn::UNDPopup::create();
        //popup->m_noElasticity = true;
        popup->show();
    }

    bool init() {
        if (!CreatorLayer::init()) {
            return false;
        }

        auto* menu = cocos2d::CCMenu::create();
        menu->setPosition(
            getContentWidth() - 42.0f,
            48.0f
        );
        menu->setContentSize({ 40.0f, 40.0f });
        menu->setID("und-menu-button-menu"_spr);

        auto* buttonSprite = cocos2d::CCSprite::createWithSpriteFrameName("logo.png"_spr);
        buttonSprite->setScale(0.4f);

        if (horn::Manager::sharedManager()->getCache().getLevels().size() == 0) {
            auto* exclamationSprite = cocos2d::CCSprite::createWithSpriteFrameName("exMark_001.png");
            exclamationSprite->setPosition({ 10.0f, 80.0f });
            buttonSprite->addChild(exclamationSprite);
        }

        auto* button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(CustomCreatorLayer::onClick)
        );
        button->setSizeMult(1.2f);
        button->setID("und-menu-button"_spr);
        button->setPosition(20.0f, 20.0f);
        menu->addChild(button);
        // so, I could add this to bottom-right-menu, but its positioning is a little weird,
        // and also that would be the ONLY thing depending on node ids in this mod, and afaik nothing else uses this spot(?)
        addChild(menu);

        return true;
    }
};
