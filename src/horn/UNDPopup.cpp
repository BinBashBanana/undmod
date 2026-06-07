#include "UNDPopup.hpp"

#include <Geode/loader/Log.hpp>

namespace horn {

static constexpr cocos2d::ccColor3B colorEnabled = { 255, 255, 255 };
static constexpr cocos2d::ccColor3B colorDisabled = { 180, 180, 180 };

void UNDPopup::clickTabAll(cocos2d::CCObject* sender) {
    if (m_body) {
        m_body->setActiveTab(UNDPopupTab::AllLevels);
    }
    m_btnSpriteTabAll->m_BGSprite->setColor(colorEnabled);
    m_btnSpriteTabPacks->m_BGSprite->setColor(colorDisabled);
    m_btnSpriteTabRandomizer->m_BGSprite->setColor(colorDisabled);
}

void UNDPopup::clickTabPacks(cocos2d::CCObject* sender) {
    if (m_body) {
        m_body->setActiveTab(UNDPopupTab::MapPacks);
    }
    m_btnSpriteTabAll->m_BGSprite->setColor(colorDisabled);
    m_btnSpriteTabPacks->m_BGSprite->setColor(colorEnabled);
    m_btnSpriteTabRandomizer->m_BGSprite->setColor(colorDisabled);
}

void UNDPopup::clickTabRandomizer(cocos2d::CCObject* sender) {
    if (m_body) {
        m_body->setActiveTab(UNDPopupTab::Randomizer);
    }
    m_btnSpriteTabAll->m_BGSprite->setColor(colorDisabled);
    m_btnSpriteTabPacks->m_BGSprite->setColor(colorDisabled);
    m_btnSpriteTabRandomizer->m_BGSprite->setColor(colorEnabled);
}

void UNDPopup::onEnter() {
    Popup::onEnter();

    // actual popup contents
    if (m_body) {
        m_body->removeFromParent();
    }
    m_body = UNDPopupBody::create(420.0f, 230.0f, m_bodyState);
    m_body->m_gameLevelCache = &m_gameLevelCache;
    m_body->setActiveTab(m_activeTab);
    m_body->setAnchorPoint({ 0.0f, 0.0f });
    m_body->setPosition(0.0f, 0.0f);
    m_bodyContainer->addChild(m_body);
}

void UNDPopup::onExit() {
    if (m_body) {
        m_activeTab = m_body->getActiveTab();
        m_bodyState = m_body->getState();
        m_body->removeFromParent();
        m_body = nullptr;
    }
    Popup::onExit();
}

bool UNDPopup::init() {
    if (!Popup::init(440, 280, "GJ_square05.png")) {
        return false;
    }

    // darken the background color
    m_bgSprite->setColor({ 128, 128, 128 });

    // modal window title
    auto* title = cocos2d::CCLabelBMFont::create("Underrated Non-Demons", "bigFont.fnt");
    title->setScale(0.42f);
    title->setAnchorPoint({ 0.0f, 0.5f });
    title->setPosition(18.0f, 260.0f);
    m_mainLayer->addChild(title);

    // body container (bug workaround)
    m_bodyContainer = cocos2d::CCNode::create();
    m_bodyContainer->setAnchorPoint({ 0.0f, 0.0f });
    m_bodyContainer->setPosition(10.0f, 10.0f);
    m_bodyContainer->setContentSize({ 420.0f, 230.0f });
    m_mainLayer->addChild(m_bodyContainer);

    // tab menu
    auto* btnMenu = cocos2d::CCMenu::create();
    btnMenu->ignoreAnchorPointForPosition(false);
    btnMenu->setAnchorPoint({ 1.0f, 0.5f });
    btnMenu->setPosition(435.0f, 260.0f);
    btnMenu->setContentSize({ 245.0f, 30.0f });
    m_mainLayer->addChild(btnMenu);

    // tab buttons
    m_btnSpriteTabAll = ButtonSprite::create("All Levels", 150.0f, true, "bigFont.fnt", "GJ_button_04.png", 0.0f, 0.8f);
    m_btnSpriteTabAll->setScale(0.45f);
    m_btnTabAll = CCMenuItemSpriteExtra::create(
        m_btnSpriteTabAll,
        this,
        menu_selector(UNDPopup::clickTabAll)
    );
    m_btnTabAll->setSizeMult(1.2f);
    m_btnTabAll->setPosition(42.5f, 15.0f);
    btnMenu->addChild(m_btnTabAll);

    m_btnSpriteTabPacks = ButtonSprite::create("Map Packs", 150.0f, true, "bigFont.fnt", "GJ_button_04.png", 0.0f, 0.8f);
    m_btnSpriteTabPacks->setScale(0.45f);
    m_btnTabPacks = CCMenuItemSpriteExtra::create(
        m_btnSpriteTabPacks,
        this,
        menu_selector(UNDPopup::clickTabPacks)
    );
    m_btnTabPacks->setSizeMult(1.2f);
    m_btnTabPacks->setPosition(122.5f, 15.0f);
    btnMenu->addChild(m_btnTabPacks);

    m_btnSpriteTabRandomizer = ButtonSprite::create("Randomizer", 150.0f, true, "bigFont.fnt", "GJ_button_04.png", 0.0f, 0.8f);
    m_btnSpriteTabRandomizer->setScale(0.45f);
    m_btnTabRandomizer = CCMenuItemSpriteExtra::create(
        m_btnSpriteTabRandomizer,
        this,
        menu_selector(UNDPopup::clickTabRandomizer)
    );
    m_btnTabRandomizer->setSizeMult(1.2f);
    m_btnTabRandomizer->setPosition(202.5f, 15.0f);
    btnMenu->addChild(m_btnTabRandomizer);

    clickTabAll(nullptr);

    return true;
}

}
