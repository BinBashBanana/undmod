#include "VersionRangeSelectorPopup.hpp"

#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

namespace horn {

bool VersionRangeSelectorPopup::init(const std::span<const std::string>& names, int initialMin, int initialMax, bool initialIsRange, const std::function<void(int, int)>& saveValues) {
    if (!Popup::init(200.0f, 240.0f, "GJ_square05.png")) {
        return false;
    }

    m_saveValues = saveValues;

    // darken the background color
    m_bgSprite->setColor({ 128, 128, 128 });

    // modal window title
    auto* title = cocos2d::CCLabelBMFont::create("Select game version", "bigFont.fnt");
    title->setScale(0.42f);
    title->setPosition(100.0f, 220.0f);
    m_mainLayer->addChild(title);

    auto* rangeLabel = cocos2d::CCLabelBMFont::create("Range", "bigFont.fnt");
    rangeLabel->setScale(0.6f);
    rangeLabel->setAnchorPoint({ 0.0f, 0.5f });
    rangeLabel->setPosition(90.0f, 185.0f);
    m_mainLayer->addChild(rangeLabel);

    m_rangeCheckbox = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(VersionRangeSelectorPopup::clickRangeCheckbox), 1.0f);
    m_rangeCheckbox->setAnchorPoint({ 1.0f, 0.5f });
    m_rangeCheckbox->setPosition(80.0f, 185.0f);
    m_buttonMenu->addChild(m_rangeCheckbox);
    m_rangeCheckbox->toggle(initialIsRange);

    m_minLabel = cocos2d::CCLabelBMFont::create("Minimum version", "bigFont.fnt");
    m_minLabel->setScale(0.4f);
    m_minLabel->setPosition(100.0f, 150.0f);
    m_mainLayer->addChild(m_minLabel);
    m_minSelector = LRSelectorEnum::create(names, initialMin, !initialIsRange, [this](int newValue) {
        if (newValue > m_maxSelector->getValue() || !m_rangeCheckbox->isToggled()) {
            m_maxSelector->setValue(newValue, false);
        }
    });
    m_minSelector->setPosition(100.0f, 125.0f);
    m_mainLayer->addChild(m_minSelector);

    m_maxLabel = cocos2d::CCLabelBMFont::create("Maximum version", "bigFont.fnt");
    m_maxLabel->setScale(0.4f);
    m_maxLabel->setPosition(100.0f, 90.0f);
    m_mainLayer->addChild(m_maxLabel);
    m_maxSelector = LRSelectorEnum::create(names, initialIsRange ? initialMax : initialMin, false, [this](int newValue) {
        if (newValue < m_minSelector->getValue()) {
            m_minSelector->setValue(newValue, false);
        }
    });
    m_maxSelector->setPosition(100.0f, 65.0f);
    m_mainLayer->addChild(m_maxSelector);

    if (!initialIsRange) {
        m_minLabel->setVisible(false);
        m_maxLabel->setVisible(false);
        m_maxSelector->setVisible(false);
    }

    auto* saveSprite = ButtonSprite::create("Save", 0.0f, false, "goldFont.fnt", "GJ_button_01.png", 0.0f, 0.8f);
    auto* saveButton = CCMenuItemSpriteExtra::create(
        saveSprite,
        this,
        menu_selector(VersionRangeSelectorPopup::clickSave)
    );
    saveButton->setSizeMult(1.2f);
    saveButton->setPosition(100.0f, 25.0f);
    m_buttonMenu->addChild(saveButton);

    return true;
}

void VersionRangeSelectorPopup::clickRangeCheckbox(cocos2d::CCObject* sender) {
    auto* target = static_cast<CCMenuItemToggler*>(sender);
    if (target->isToggled()) {
        // single version
        m_minLabel->setVisible(false);
        m_maxLabel->setVisible(false);
        m_maxSelector->setVisible(false);
        m_minSelector->m_wrap = true;
    } else {
        // version range
        m_minLabel->setVisible(true);
        m_maxLabel->setVisible(true);
        m_maxSelector->setVisible(true);
        m_minSelector->m_wrap = false;
    }
}

void VersionRangeSelectorPopup::clickSave(cocos2d::CCObject* sender) {
    if (m_saveValues) {
        m_saveValues(m_minSelector->getValue(), m_rangeCheckbox->isToggled() ? m_maxSelector->getValue() : m_minSelector->getValue());
    }
    onClose(nullptr);
}

}
