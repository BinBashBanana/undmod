#pragma once

#include <Geode/loader/Mod.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/async.hpp>
#include <functional>

#include "Cache.hpp"

namespace horn {

//! @brief Shit manager.
class Manager final {
public:
    //! @brief Get manager singleton.
    //! @return Manager singleton.
    static Manager* sharedManager();

    //! @brief Initialize manager.
    //! @return Whether the initialization was successful.
    bool init();

    //! @brief Attempt to refresh data.
    void refresh();

    //! @brief Load/reload mod settings.
    void loadSettings();

    //! @brief Download and update cache.
    //! @param finished Download finished callback.
    //! @param failed Download failed callback.
    void download(
        std::function<void()> finished,
        std::function<void()> failed
    );

    //! @brief Get hint shown.
    //! @return Setting value.
    bool getHintShown() const {
        return geode::Mod::get()->getSavedValue<bool>("hint-shown");
    }

    //! @brief Set hint shown.
    //! @param hintShown Whether the hint was shown.
    void setHintShown(bool hintShown) {
        geode::Mod::get()->setSavedValue<bool>("hint-shown", hintShown);
    }

    Cache const& getCache() const { return m_cache; }

    bool showHorns() const { return m_showHorns; }
    bool showTierText() const { return m_showTierText; }

private:
    //! @brief Get internal spreadsheet ID.
    //! @return Spreadsheet ID.finished
    std::string getSheetID() const;

    //! @brief Update cache from response data.
    //! @param str Response data.
    void cache(std::string const& str);

    static Manager* s_sharedManager;

    geode::async::TaskHolder<geode::utils::web::WebResponse> s_fetchListener;

    Cache m_cache;

    bool m_showHorns;
    bool m_showTierText;
};

} // namespace horn
