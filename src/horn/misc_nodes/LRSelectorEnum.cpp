#include "LRSelectorEnum.hpp"

namespace horn {

int LRSelectorEnum::getValue() {
    return m_value;
}

void LRSelectorEnum::setValue(int value, bool triggerCallback) {
    int newValue = std::clamp(value, 0, m_max);
    if (m_value != newValue) {
        m_label->setString(m_names[newValue].c_str());
        m_label->limitLabelWidth(m_boxWidth - 10.0f, 0.6f, 0.001f);
        m_value = newValue;
        if (triggerCallback && m_onChange) {
            m_onChange(m_value);
        }
    }
}

bool LRSelectorEnum::init(const std::span<const std::string>& names, int initial, bool wrap, const std::function<void(int)>& onChange, float boxWidth) {
    if (!LRSelector<int>::init(onChange, boxWidth)) {
        return false;
    }

    m_names = names;
    m_max = m_names.size() - 1;
    if (m_max == -1) {
        return false;
    }
    m_initial = std::clamp(initial, 0, m_max);
    m_value = m_initial;
    m_wrap = wrap;

    m_label = cocos2d::CCLabelBMFont::create(m_names[m_value].c_str(), "bigFont.fnt");
    m_label->limitLabelWidth(boxWidth - 10.0f, 0.6f, 0.001f);
    m_label->setPosition(boxWidth / 2.0f + 20.0f, 15.0f);
    addChild(m_label);

    return true;
}

void LRSelectorEnum::onClickArrow(cocos2d::CCObject* sender) {
    int mod = sender->getTag();
    int newValue = m_value + mod;
    if (newValue > m_max) {
        newValue = m_wrap ? 0 : m_max;
    } else if (newValue < 0) {
        newValue = m_wrap ? m_max : 0;
    }
    if (m_value != newValue) {
        m_label->setString(m_names[newValue].c_str());
        m_label->limitLabelWidth(m_boxWidth - 10.0f, 0.6f, 0.001f);
        m_value = newValue;
        if (m_onChange) {
            m_onChange(m_value);
        }
    }
}

}
