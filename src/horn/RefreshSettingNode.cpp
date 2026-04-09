#include "RefreshSettingNode.hpp"

#include <Geode/binding/FLAlertLayer.hpp>
#include "RefreshSettingValue.hpp"
#include "Manager.hpp"

namespace horn {

bool RefreshSettingNode::init(std::shared_ptr<RefreshSettingValue> setting, float width) {
    if (!SettingNodeV3::init(setting, width)) {
        return false;
    }

    this->getButtonMenu()->setContentWidth(20.0f);
    auto pos = this->getButtonMenu()->getContentSize() / 2.0f;

    m_buttonSprite = cocos2d::CCSprite::createWithSpriteFrameName(
        "GJ_updateBtn_001.png"
    );
    m_buttonSprite->setScale(0.5f);

    m_button = CCMenuItemSpriteExtra::create(
        m_buttonSprite,
        this,
        menu_selector(RefreshSettingNode::onRefresh)
    );
    m_button->setPosition(pos);
    this->getButtonMenu()->addChild(m_button);

    m_spinner = geode::LoadingSpinner::create(20.0f);
    m_spinner->setVisible(false);

    m_spinnerHolder = cocos2d::CCMenuItem::create();
    m_spinnerHolder->setContentSize({ 20.0f, 20.0f });
    m_spinnerHolder->setPosition(pos);
    m_spinner->setPosition({
        m_spinnerHolder->getContentSize() / 2.0f
    });

    m_spinnerHolder->addChild(m_spinner);
    this->getButtonMenu()->addChild(m_spinnerHolder);

    return true;
}

void RefreshSettingNode::onRefresh(cocos2d::CCObject* sender) {
    m_button->setVisible(false);
    m_spinner->setVisible(true);

    Manager::sharedManager()->download(
        [this] {
            m_spinner->setVisible(false);
            m_button->setVisible(true);
        },
        [this] {
            m_spinner->setVisible(false);
            m_button->setVisible(true);

            FLAlertLayer::create(
                "Error",
                "Download failed. Please try again later.",
                "OK"
            )->show();
        }
    );
}

} // namespace horn
