#pragma once

#include <Geode/binding/LevelManagerDelegate.hpp>
#include <Geode/binding/GameLevelManager.hpp>

namespace horn {

//! @brief Interface for downloading level info.
class LevelInfoDownloader : public LevelManagerDelegate {
public:
    static void download(GJSearchObject* search, const std::function<void(cocos2d::CCArray*)>& finished, const std::function<void()>& failed) {
        auto delegate = new LevelInfoDownloader();
        delegate->m_gameLevelManager = GameLevelManager::sharedState();
        if (delegate->m_gameLevelManager->m_levelManagerDelegate) {
            failed();
            delete delegate;
            return;
        }
        delegate->m_finished = finished;
        delegate->m_failed = failed;
        delegate->m_gameLevelManager->m_levelManagerDelegate = delegate;
        delegate->m_gameLevelManager->getOnlineLevels(search);
    }

private:
    GameLevelManager* m_gameLevelManager;
    std::function<void(cocos2d::CCArray*)> m_finished;
    std::function<void()> m_failed;

    void loadLevelsFinished(cocos2d::CCArray* levels, const char*, int) override {
        m_gameLevelManager->m_levelManagerDelegate = nullptr;
        m_finished(levels);
        delete this;
    }

    void loadLevelsFailed(const char*, int) override {
        m_gameLevelManager->m_levelManagerDelegate = nullptr;
        m_failed();
        delete this;
    }
};

}
