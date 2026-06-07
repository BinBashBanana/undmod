#pragma once

#include <span>
#include <Geode/utils/general.hpp>

#include "LRSelector.hpp"

namespace horn {

//! @brief Form node with a left and right button (enum version).
class LRSelectorEnum : public LRSelector<int> {
public:
    static LRSelectorEnum* create(const std::span<const std::string> names, int initial, bool wrap, const std::function<void(int)>& onChange, float boxWidth = 80.0f) {
        auto ret = new LRSelectorEnum();
        if (ret->init(names, initial, wrap, onChange, boxWidth)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    int getValue() override;

    void setValue(int value, bool triggerCallback) override;

    int m_max;
    int m_initial;
    bool m_wrap;

private:
    bool init(const std::span<const std::string>& names, int initial, bool wrap, const std::function<void(int)>& onChange, float boxWidth);

    void onClickArrow(cocos2d::CCObject* sender) override;

    cocos2d::CCLabelBMFont* m_label;
    std::span<const std::string> m_names;

    int m_value;
};

}
