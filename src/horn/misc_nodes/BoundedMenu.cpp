#include "BoundedMenu.hpp"

#include <Geode/cocos/touch_dispatcher/CCTouchDispatcher.h>

namespace horn {

void BoundedMenu::setTouchRectNode(cocos2d::CCNode* node) {
    m_touchRectNode = node;
}

void BoundedMenu::clearTouchRectNode() {
    m_touchRectNode = nullptr;
}

bool BoundedMenu::ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) {
    if (m_touchRectNode && !m_touchRectNode->boundingBox().containsPoint(m_touchRectNode->getParent()->convertTouchToNodeSpace(pTouch))) {
        return false;
    }
    return cocos2d::CCMenu::ccTouchBegan(pTouch, pEvent);
}

void BoundedMenu::registerWithTouchDispatcher() {
    auto* dispatcher = cocos2d::CCTouchDispatcher::get();
    dispatcher->addTargetedDelegate(this, dispatcher->getTargetPrio(), false);
}

}
