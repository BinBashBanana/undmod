#pragma once

#include <Geode/utils/general.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/TextInputDelegate.hpp>

#include "LRSelector.hpp"

namespace horn {

//! @brief Form node with a left and right button (int version, optionally editable with keyboard).
class LRSelectorInt : public LRSelector<int>, public TextInputDelegate {
public:
    static LRSelectorInt* create(int min, int max, int initial, int step, bool enforceStep, bool wrap, bool enableTyping, const std::function<void(int)>& onChange, float boxWidth = 40.0f) {
        auto ret = new LRSelectorInt();
        if (ret->init(min, max, initial, step, enforceStep, wrap, enableTyping, onChange, boxWidth)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    int getValue() override;

    void setValue(int value, bool triggerCallback) override;

    //! @brief Set m_step and optionally trigger m_onChange if this caused the value to change. 
    void setStep(int step, bool triggerCallback);

    int m_min;
    int m_max;
    int m_initial;
    int m_step;
    bool m_enforceStep;
    bool m_wrap;

private:
    bool init(int min, int max, int initial, int step, bool enforceStep, bool wrap, bool enableTyping, const std::function<void(int)>& onChange, float boxWidth);
    void onClickArrow(cocos2d::CCObject* sender) override;
    void textInputClosed(CCTextInputNode* input) override;
    int roundToStep(int base);

    geode::TextInput* m_textInput;

    int m_value;
};

}
