#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

namespace horn {

//! @brief Base class for form nodes with a left and right button.
template <typename T>
class LRSelector : public cocos2d::CCNode {
public:
    virtual ~LRSelector() = default;

    std::function<void(T)> m_onChange;

    virtual T getValue() = 0;
    virtual void setValue(T, bool) = 0;

protected:
    bool init(const std::function<void(T)>& onChange, float boxWidth) {
        if (!cocos2d::CCNode::init()) {
            return false;
        }

        setAnchorPoint({ 0.5f, 0.5f });

        m_onChange = onChange;
        m_boxWidth = boxWidth;
        float nodeWidth = boxWidth + 40.0f;
        setContentSize({ nodeWidth, 30.0f });

        auto* m_background = geode::NineSlice::create("square02_small.png");
        m_background->setOpacity(100);
        m_background->setContentSize({ boxWidth, 30.0f });
        m_background->setPosition(nodeWidth / 2.0f, 15.0f);
        addChild(m_background);

        m_menu = cocos2d::CCMenu::create();
        m_menu->setContentSize({ nodeWidth, 30.0f });
        m_menu->setPosition(0.0f, 0.0f);
        addChild(m_menu);

        auto* spriteL = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        spriteL->setScale(0.96f);
        auto* buttonL = CCMenuItemSpriteExtra::create(
            spriteL,
            this,
            menu_selector(LRSelector::onClickArrow)
        );
        buttonL->setSizeMult(1.2);
        buttonL->setTag(-1);
        buttonL->setPosition(10.0f, 15.0f);
        m_menu->addChild(buttonL);

        auto* spriteR = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        spriteR->setScale(0.96f);
        auto* buttonR = CCMenuItemSpriteExtra::create(
            spriteR,
            this,
            menu_selector(LRSelector::onClickArrow)
        );
        buttonR->setSizeMult(1.2);
        buttonR->setTag(1);
        buttonR->setPosition(nodeWidth - 10.0f, 15.0f);
        m_menu->addChild(buttonR);

        return true;
    }

    virtual void onClickArrow(cocos2d::CCObject* sender) = 0;

    cocos2d::CCMenu* m_menu;
    float m_boxWidth;
};

}
