#pragma once

#include <Geode/cocos/menu_nodes/CCMenu.h>

namespace horn {

//! @brief CCMenu that only accepts clicks when they also intersect with a specified parent or grandparent node.
class BoundedMenu : public cocos2d::CCMenu {
public:
    static BoundedMenu* create() {
        auto ret = new BoundedMenu();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    //! @brief Set which node this menu should accept clicks in. node should be a parent or grandparent of this node.
    void setTouchRectNode(cocos2d::CCNode* node);

    //! @brief Make the menu accept all clicks.
    void clearTouchRectNode();

private:
    bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;
    void registerWithTouchDispatcher() override;

    cocos2d::CCNode* m_touchRectNode = nullptr;
};

}
