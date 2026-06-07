#include "RefreshSettingNode.hpp"

#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/ui/Notification.hpp>
#include "RefreshSettingValue.hpp"
#include "Manager.hpp"

namespace horn {

bool RefreshSettingNode::init(std::shared_ptr<RefreshSettingValue> setting, float width) {
    if (!SettingNodeV3::init(setting, width)) {
        return false;
    }

    auto* menu = getButtonMenu();
    menu->setContentWidth(20.0f);
    auto pos = menu->getContentSize() / 2.0f;

    auto* buttonSprite = cocos2d::CCSprite::createWithSpriteFrameName(
        "GJ_updateBtn_001.png"
    );
    buttonSprite->setScale(0.5f);

    m_button = CCMenuItemSpriteExtra::create(
        buttonSprite,
        this,
        menu_selector(RefreshSettingNode::onRefresh)
    );
    m_button->setPosition(pos);
    menu->addChild(m_button);

    m_spinner = geode::LoadingSpinner::create(20.0f);
    m_spinner->setVisible(false);

    auto* spinnerHolder = cocos2d::CCMenuItem::create();
    spinnerHolder->setContentSize({ 20.0f, 20.0f });
    spinnerHolder->setPosition(pos);
    m_spinner->setPosition({
        spinnerHolder->getContentSize() / 2.0f
    });

    spinnerHolder->addChild(m_spinner);
    menu->addChild(spinnerHolder);

    return true;
}

void RefreshSettingNode::onRefresh(cocos2d::CCObject* sender) {
    m_button->setVisible(false);
    m_spinner->setVisible(true);

    Manager::sharedManager()->download(
        [this] {
            m_spinner->setVisible(false);
            m_button->setVisible(true);

            geode::Notification::create("Successfully refreshed", geode::NotificationIcon::Success, 1.0f)->show();
        },
        [this](std::string err) {
            m_spinner->setVisible(false);
            m_button->setVisible(true);

            FLAlertLayer::create(
                "Error",
                fmt::format("Download failed. Please try again later.\nMessage: {}", err),
                "OK"
            )->show();
        }
    );
}

} // namespace horn
