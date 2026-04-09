#include "Manager.hpp"

#include <Geode/loader/Log.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/utils/string.hpp>
#include <fmt/format.h>

#include <string>

namespace horn {

Manager* Manager::s_sharedManager = nullptr;

Manager* Manager::sharedManager() {
    if (s_sharedManager == nullptr) {
        s_sharedManager = new (std::nothrow) Manager();
        s_sharedManager->init();
    }

    return s_sharedManager;
}

bool Manager::init() {
    m_cache = Cache(geode::Mod::get()->getSavedValue<matjson::Value>("cache"));
    loadSettings();

    return true;
}

static std::string urlencode(std::string const& str) {
    // I see no problems with this.
    return geode::utils::string::replace(str, " ", "%20");
}

void Manager::refresh() {
    std::time_t now = std::time(nullptr);

    // Check every 7 days.
    if (m_cache.getTimestamp() + 604800 < now) {
        download([]{}, []{});
    }
}

void Manager::download(
    std::function<void()> finished,
    std::function<void()> failed
) {
    geode::log::info("Downloading level info");
    std::string url;

    if (geode::Mod::get()->getSettingValue<bool>("enable-alternate-sheet-url")) {
        url = geode::Mod::get()->getSettingValue<std::string>("alternate-sheet-url");
    } else {
        std::string sheet = "All levels in chronological order";
        std::string tqx = "out:csv";
        std::string tq = "select F, C, G, H";
        std::string query = fmt::format(
            "sheet={}&tqx={}&tq={}",
            sheet,
            tqx,
            tq
        );

        url = fmt::format(
            "https://docs.google.com/spreadsheets/d/{}/gviz/tq?{}",
            getSheetID(),
            urlencode(query)
        );
    }

    auto req = geode::utils::web::WebRequest();
    s_fetchListener.spawn(
        req.get(url),
        [=, this](geode::utils::web::WebResponse res) {
            if (res.ok()) {
                cache(res.string().unwrap());
                finished();
            } else {
                failed();
            }
        }
    );
}

std::string Manager::getSheetID() const {
    return "1-Abvx7zXRAqpGFVbdTXpn6g1TRYp9WuZqW2pHWE7Dr4";
}

void Manager::cache(std::string const& str) {
    m_cache = Cache(str);
    geode::Mod::get()->setSavedValue("cache", m_cache.json());
}

void Manager::loadSettings() {
    m_showHorns = geode::Mod::get()->getSettingValue<bool>("show-horns");
    m_showTierText = geode::Mod::get()->getSettingValue<bool>("show-tier-text");
}

} // namespace horn

$on_mod(Loaded) {
    horn::Manager::sharedManager()->refresh();
    geode::listenForSettingChanges<bool>("show-horns", [](bool v) {
        horn::Manager::sharedManager()->loadSettings();
    });
    geode::listenForSettingChanges<bool>("show-tier-text", [](bool v) {
        horn::Manager::sharedManager()->loadSettings();
    });
}
