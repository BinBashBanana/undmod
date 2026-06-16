#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>

#include "misc_nodes/LRSelectorEnum.hpp"

namespace horn {

//! @brief Popup modal window for selecting a range of Geometry Dash game versions.
class VersionRangeSelectorPopup : public geode::Popup {
public:
    static VersionRangeSelectorPopup* create(const std::span<const std::string>& names, int initialMin, int initialMax, bool initialIsRange, const std::function<void(int, int)>& saveValues) {
        auto ret = new VersionRangeSelectorPopup();
        if (ret->init(names, initialMin, initialMax, initialIsRange, saveValues)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

private:
    bool init(const std::span<const std::string>& names, int initialMin, int initialMax, bool initialIsRange, const std::function<void(int, int)>& saveValues);
    void clickRangeCheckbox(cocos2d::CCObject* sender);
    void clickSave(cocos2d::CCObject* sender);

    std::function<void(int, int)> m_saveValues;
    cocos2d::CCLabelBMFont* m_minLabel;
    cocos2d::CCLabelBMFont* m_maxLabel;
    LRSelectorEnum* m_minSelector;
    LRSelectorEnum* m_maxSelector;
    CCMenuItemToggler* m_rangeCheckbox;
};

}
