#include "Manager.hpp"

#include <Geode/loader/Log.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/utils/string.hpp>
#include <fmt/format.h>

#include <string>

#include "CSV.hpp"

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

    std::ifstream packsJson(geode::Mod::get()->getResourcesDir() / "packs.json");
    m_packs = matjson::parse(packsJson).unwrapOrDefault();

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
        download([]{}, [](std::string err) {
            geode::log::error("Failed to download sheet data: {}", err);
        });
    }
}

void Manager::download(
    std::function<void()> finished,
    std::function<void(std::string)> failed
) {
    geode::log::info("Downloading level info");
    std::string url;

    if (geode::Mod::get()->getSettingValue<bool>("enable-alternate-sheet-url")) {
        url = geode::Mod::get()->getSettingValue<std::string>("alternate-sheet-url");
    } else {
        std::string sheet = "All levels in chronological order";
        std::string tqx = "out:csv";
        std::string tq = "select F, C, B, G, H";
        std::string query = fmt::format(
            "sheet={}&tqx={}&tq={}",
            sheet,
            tqx,
            tq
        );

        url = fmt::format(
            "https://docs.google.com/spreadsheets/d/{}/gviz/tq?{}",
            "1-Abvx7zXRAqpGFVbdTXpn6g1TRYp9WuZqW2pHWE7Dr4",
            urlencode(query)
        );
    }

    auto req = geode::utils::web::WebRequest();
    s_fetchListener.spawn(
        req.get(url),
        [=, this](geode::utils::web::WebResponse res) {
            if (!res.ok()) {
                failed(res.string().unwrapOr("Unknown error"));
                return;
            }
            auto strVal = res.string();
            if (!strVal.isOk()) {
                failed("Unknown error 2");
                return;
            }
            auto str = strVal.unwrap();
            auto pos = str.find("\n");
            if (pos == std::string::npos) {
                failed("Missing sheet header");
                return;
            }
            auto header = CSV(str.substr(0, pos)).rows()[0];
            if (header.size() == 5
                && header[0] == "Level ID"
                && header[1] == "Tier"
                && header[2] == "Difficulty"
                && header[3].starts_with("Skillset")
                && header[4].starts_with("Description")
            ) {
                cache(str);
                finished();
            } else {
                failed("Bad sheet header");
            }
        }
    );
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
