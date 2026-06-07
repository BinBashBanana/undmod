#include "ProgressBar.hpp"

namespace horn {

bool ProgressBar::init(float width, float height, cocos2d::ccColor4F backgroundColor, cocos2d::ccColor4F barColor, float value) {
    if (!cocos2d::CCNode::init()) {
        return false;
    }

    setContentSize({ width, height });
    m_backgroundColor = backgroundColor;
    m_barColor = barColor;
    m_value = value;

    return true;
}

void ProgressBar::draw() {
    ccDrawSolidRect({ 0.0f, 0.0f }, getContentSize(), m_backgroundColor);
    if (m_value > 0.0f) {
        ccDrawSolidRect({ 0.0f, 0.0f }, { getContentWidth() * m_value, getContentHeight() }, m_barColor);
    }
}

}
