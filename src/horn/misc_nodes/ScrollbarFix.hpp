#pragma once

#include <Geode/ui/Scrollbar.hpp>
#include <Geode/utils/cocos.hpp>

namespace horn {

//! @brief Bugfixed geode::Scrollbar.
class ScrollbarFix : public geode::Scrollbar {
public:
    static ScrollbarFix* create(CCScrollLayerExt* list) {
        auto ret = new ScrollbarFix();
        if (ret->init(list)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

private:
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
        // just a bugfix
        if (!geode::cocos::nodeIsVisible(this)) {
            return false;
        }
        return geode::Scrollbar::ccTouchBegan(touch, event);
    }
};

}
