#pragma once

#include <Geode/cocos/menu_nodes/CCMenuItem.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

namespace horn {

//! @brief Menu item with option to tint when selected, and extra data members.
class UNDMenuItem : public cocos2d::CCMenuItem {
public:
    static UNDMenuItem* create(cocos2d::CCObject* rec, cocos2d::SEL_MenuHandler selector) {
        auto ret = new UNDMenuItem();
        if (ret->initWithTarget(rec, selector)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    int m_index1;
    int m_index2;
    bool m_isPack = false;
    bool m_tintSelected = false;

private:
    void selected() override;

    void unselected() override;

    cocos2d::CCLayerColor* m_tintLayer = nullptr;
};

}
