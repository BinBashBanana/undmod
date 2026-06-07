#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/draw_nodes/CCDrawingPrimitives.h>

namespace horn {

//! @brief Simple rectangular progress bar node.
class ProgressBar : public cocos2d::CCNode {
public:
    static ProgressBar* create(float width, float height, cocos2d::ccColor4F backgroundColor, cocos2d::ccColor4F barColor, float value) {
        auto ret = new ProgressBar();
        if (ret->init(width, height, backgroundColor, barColor, value)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    cocos2d::ccColor4F m_backgroundColor;
    cocos2d::ccColor4F m_barColor;
    float m_value;

private:
    bool init(float width, float height, cocos2d::ccColor4F backgroundColor, cocos2d::ccColor4F barColor, float value);

    void draw() override;
};

}
