#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/Scrollbar.hpp>
#include <Geode/ui/LoadingSpinner.hpp>
#include <Geode/utils/cocos.hpp>

#include "LevelInfo.hpp"
#include "misc_nodes/LRSelectorEnum.hpp"
#include "misc_nodes/LRSelectorInt.hpp"

namespace horn {

//! @brief Tabs of UND popup window.
enum class UNDPopupTab {
    None = -1,
    AllLevels = 0,
    MapPacks = 1,
    Randomizer = 2
};

//! @brief How to sort a list of levels.
enum class LevelSortMode {
    None = 0,
    LevelID = 1,
    LevelName = 2
};

//! @brief Map pack information.
struct UNDMapPack {
    std::string name;
    cocos2d::ccColor3B color;
    std::vector<int> levels;
};

//! @brief Represents the state of all the form elements in UNDPopupBody.
struct UNDPopupBodyState {
    float scrollHeight = -1.0f;
    int difficultyMin = 0;
    int difficultyMax = 5;
    int tierMin = 1;
    int tierMax = 5;
    int randomCount = 50;
    bool uncompletedOnly = false;
    bool equalSplit = false;
    bool sortByID = false;
};

//! @brief Main content of UND Popup window.
class UNDPopupBody : public cocos2d::CCNode {
public:
    static UNDPopupBody* create(float width, float height, const UNDPopupBodyState& state = {}) {
        auto ret = new UNDPopupBody();
        if (ret->init(width, height, state)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    //! @brief Set the active tab.
    void setActiveTab(UNDPopupTab tab);

    //! @brief Return the currently active tab.
    UNDPopupTab getActiveTab() const { return m_activeTab; }

    //! @brief Get the state of all form elements.
    UNDPopupBodyState getState() const;

    std::unordered_map<int, geode::Ref<GJGameLevel>>* m_gameLevelCache = nullptr;

private:
    bool init(float width, float height, const UNDPopupBodyState& state);

    //! @brief Populate main body contents.
    void initMainContent();

    //! @brief Open a pack list but download level info into cache first if necessary.
    void openPackList(const std::vector<int>& levels, const std::vector<std::vector<int>>& levelLists, const std::string& name, cocos2d::ccColor3B color, LevelSortMode sort);

    //! @brief Open a pack list using data from m_gameLevelCache.
    void openPackListFromCache(const std::vector<std::vector<int>>& levelLists, const std::string& name, cocos2d::ccColor3B color, LevelSortMode sort);

    //! @brief Click the refresh button to download sheet data.
    void clickRefresh(cocos2d::CCObject* sender);

    void openSheetInBrowser(cocos2d::CCObject* sender);
    void clickListTrigger(cocos2d::CCObject* sender);
    void clickEqualSplit(cocos2d::CCObject* sender);
    void clickGoRandomizer(cocos2d::CCObject* sender);

    //! @brief Update the step value of m_randomCountSelector based on the value of the randomizer form elements.
    void updateRandomizerCountStep(bool enabled);

    float m_width;
    float m_height;

    bool m_downloadTabActive;
    UNDPopupTab m_activeTab;
    cocos2d::CCNode* m_tabDownload;
    cocos2d::CCNode* m_tabAll;
    cocos2d::CCNode* m_tabPacks;
    cocos2d::CCNode* m_tabRandomizer;

    CCMenuItemSpriteExtra* m_downloadButton;
    geode::LoadingSpinner* m_downloadSpinner;

    CCMenuItemToggler* m_checkboxSortByID = nullptr;

    geode::ScrollLayer* m_scrollLayer = nullptr;

    LRSelectorEnum* m_difficultyMinSelector = nullptr;
    LRSelectorEnum* m_difficultyMaxSelector = nullptr;
    LRSelectorInt* m_tierMinSelector = nullptr;
    LRSelectorInt* m_tierMaxSelector = nullptr;
    LRSelectorInt* m_randomCountSelector = nullptr;
    CCMenuItemToggler* m_checkboxUncompleted = nullptr;
    CCMenuItemToggler* m_checkboxEqualSplit = nullptr;

    // difficulty -> tier -> levels
    std::array<std::array<std::vector<int>, 6>, 6> m_sortedLevels;
    std::unordered_map<int, bool> m_levelCompletion;
    std::vector<UNDMapPack> m_sortedPacks;

    UNDPopupBodyState m_initialState;
};

}
