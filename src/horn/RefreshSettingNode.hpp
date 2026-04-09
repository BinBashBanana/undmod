#pragma once

#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/LoadingCircle.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/cocos/menu_nodes/CCMenuItem.h>

namespace horn {

class RefreshSettingValue;

//! @brief Refresh setting node.
class RefreshSettingNode : public geode::SettingNodeV3 {
public:
    //! @brief Create refresh setting node.
    //! @param setting Setting value.
    //! @param width Width of settings layer.
    //! @return Refresh button.
    static RefreshSettingNode* create(std::shared_ptr<RefreshSettingValue> setting, float width) {
        auto ret = new RefreshSettingNode();
        if (ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool hasUncommittedChanges() const override { return false; }
    bool hasNonDefaultValue() const override { return false; }

    std::shared_ptr<RefreshSettingValue> getSetting() const {
        return std::static_pointer_cast<RefreshSettingValue>(SettingNodeV3::getSetting());
    }

private:
    bool init(std::shared_ptr<RefreshSettingValue> setting, float width);

    //! @brief Refresh callback.
    //! @param sender Sender.
    void onRefresh(cocos2d::CCObject* sender);

    void onCommit() override {}
    void onResetToDefault() override {}

    cocos2d::CCSprite* m_buttonSprite;
    CCMenuItemSpriteExtra* m_button;
    cocos2d::CCMenuItem* m_spinnerHolder;
    geode::LoadingSpinner* m_spinner;
};

} // namespace horn
