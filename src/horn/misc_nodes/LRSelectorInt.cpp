#include "LRSelectorInt.hpp"

namespace horn {

int LRSelectorInt::getValue() {
    return m_value;
}

void LRSelectorInt::setValue(int value, bool triggerCallback) {
    int newValue = std::clamp(value, m_min, m_max);
    if (m_enforceStep) {
        newValue = roundToStep(newValue);
    }
    if (m_value != newValue) {
        m_textInput->setString(geode::utils::numToString<int>(newValue), false);
        m_value = newValue;
        if (triggerCallback && m_onChange) {
            m_onChange(m_value);
        }
    }
}

void LRSelectorInt::setStep(int step, bool triggerCallback) {
    m_step = step;
    if (m_enforceStep) {
        int newValue = roundToStep(m_value);
        if (m_value != newValue) {
            m_textInput->setString(geode::utils::numToString<int>(newValue), false);
            m_value = newValue;
            if (triggerCallback && m_onChange) {
                m_onChange(m_value);
            }
        }
    }
}

bool LRSelectorInt::init(int min, int max, int initial, int step, bool enforceStep, bool wrap, bool enableTyping, const std::function<void(int)>& onChange, float boxWidth) {
    if (!LRSelector<int>::init(onChange, boxWidth)) {
        return false;
    }

    m_min = min;
    m_max = max;
    m_step = step;
    m_enforceStep = enforceStep;
    m_wrap = wrap;
    m_initial = std::clamp(initial, min, max);
    if (enforceStep) {
        m_initial = roundToStep(m_initial);
    }
    m_value = m_initial;

    m_textInput = geode::TextInput::create(boxWidth, "", "bigFont.fnt");
    m_textInput->hideBG();
    m_textInput->setPosition(boxWidth / 2.0f + 20.0f, 15.0f);
    m_textInput->setCommonFilter(geode::CommonFilter::Int);
    m_textInput->setMaxCharCount(11);
    m_textInput->setString(geode::utils::numToString<int>(initial), false);
    m_textInput->setDelegate(this);
    addChild(m_textInput);
    if (!enableTyping) {
        m_textInput->getInputNode()->setTouchEnabled(false);
    }

    return true;
}

void LRSelectorInt::onClickArrow(cocos2d::CCObject* sender) {
    int mod = sender->getTag() * m_step;
    int newValue = m_value + mod;
    if (newValue > m_max) {
        newValue = m_wrap ? m_min : m_max;
    } else if (newValue < m_min) {
        newValue = m_wrap ? m_max : m_min;
    }
    if (m_enforceStep) {
        newValue = roundToStep(newValue);
    }
    if (m_value != newValue) {
        m_textInput->setString(geode::utils::numToString<int>(newValue), false);
        m_value = newValue;
        if (m_onChange) {
            m_onChange(m_value);
        }
    }
}

void LRSelectorInt::textInputClosed(CCTextInputNode* input) {
    bool forceUpdate = false;
    int contentValue;
    auto contentTmp = geode::utils::numFromString<int>(input->getString());
    if (contentTmp.isOk()) {
        contentValue = contentTmp.unwrap();
    } else {
        contentValue = m_initial;
        forceUpdate = true;
    }
    int newValue = std::clamp(contentValue, m_min, m_max);
    if (m_enforceStep) {
        newValue = roundToStep(newValue);
    }
    if (contentValue != newValue || forceUpdate) {
        m_textInput->setString(geode::utils::numToString<int>(newValue), false);
    }
    if (m_value != newValue) {
        m_value = newValue;
        if (m_onChange) {
            m_onChange(m_value);
        }
    }
}

int LRSelectorInt::roundToStep(int base) {
    int rem = base % m_step;
    if (!rem) {
        return base;
    }
    base -= rem;
    if (std::abs(rem) >= m_step / 2) {
        if (rem > 0) {
            base += m_step;
        } else {
            base -= m_step;
        }
    }
    // account for under/overshoot, assume base argument is already clamped
    if (base < m_min) {
        base += m_step;
    } else if (base > m_max) {
        base -= m_step;
    }
    return base;
}

}
