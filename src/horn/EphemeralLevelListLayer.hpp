#pragma once


#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>

namespace horn {

//! @brief A simplified version of LevelListLayer that also updates on onEnter correctly.
class EphemeralLevelListLayer : public cocos2d::CCLayer {
public:
    static EphemeralLevelListLayer* create(cocos2d::CCArray* levels, const std::string& name, cocos2d::ccColor3B barColor) {
        auto ret = new EphemeralLevelListLayer();
        if (ret->init(levels, name, barColor)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

private:
    ~EphemeralLevelListLayer();

    bool init(cocos2d::CCArray* levels, const std::string& name, cocos2d::ccColor3B barColor);
    void onClickBack(cocos2d::CCObject* sender);
    void keyBackClicked() override;
    void onClickCopy(cocos2d::CCObject* sender);
    void onEnter() override;
    void onExit() override;

    cocos2d::CCArray* m_levels = nullptr;
    cocos2d::CCNode* m_refreshingContent = nullptr;
    cocos2d::ccColor3B m_barColor;
    std::string m_name;
};

}
