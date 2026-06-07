#include "UNDMenuItem.hpp"

namespace horn {

void UNDMenuItem::selected() {
    if (!m_tintSelected) {
        return;
    }
    if (m_tintLayer) {
        m_tintLayer->setContentSize(getContentSize());
        m_tintLayer->setVisible(true);
    } else {
        m_tintLayer = cocos2d::CCLayerColor::create({ 0, 0, 0, 32 }, getContentWidth(), getContentHeight());
        m_tintLayer->setAnchorPoint({ 0.0f, 0.0f });
        m_tintLayer->setContentSize(getContentSize());
        m_tintLayer->setPosition(0.0f, 0.0f);
        addChild(m_tintLayer, 1000);
    }
}

void UNDMenuItem::unselected() {
    if (m_tintLayer) {
        m_tintLayer->setVisible(false);
    }
}

}
