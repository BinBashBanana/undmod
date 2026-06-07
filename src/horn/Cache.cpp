#include "Cache.hpp"

#include <Geode/loader/Log.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/utils/general.hpp>

#include "CSV.hpp"

#define CACHE_VERSION 2

namespace horn {

Cache::Cache(std::string const& str) {
    m_timestamp = std::time(nullptr);
    
    CSV csv(str);
    auto rows = csv.rows();

    // First row is the header. Skip it.
    for (int i = 1; i < rows.size(); i++) {
        auto row = rows[i];

        int levelID = geode::utils::numFromString<int>(row[0]).unwrapOrDefault();
        m_levels.emplace(levelID, row);
    }
}

Cache::Cache(matjson::Value const& json) {
    m_timestamp = json["timestamp"].asInt().unwrapOr(0);
    // discard if timestamp is missing or invalid
    if (m_timestamp <= 0) {
        return;
    }
    // discard if version is missing or different
    if (json["version"].asInt().unwrapOr(0) != CACHE_VERSION) {
        m_timestamp = 0;
        return;
    }

    for (auto& [k, v] : json["levels"]) {
        int levelID = geode::utils::numFromString<int>(k).unwrapOrDefault();
        m_levels.emplace(levelID, v);
    }
}

matjson::Value Cache::json() const {
    matjson::Value levels;
    for (auto [levelID, info] : m_levels) {
        std::string k = std::to_string(levelID);

        levels[k] = info;
    }

    matjson::Value res;
    res["version"] = CACHE_VERSION;
    res["timestamp"] = m_timestamp;
    res["levels"] = levels;

    return res;
}

std::optional<LevelInfo> Cache::getLevelInfo(int levelID) const {
    auto info = m_levels.find(levelID);
    if (info == m_levels.end()) {
        return std::nullopt;
    }
    return info->second;
}

} // namespace horn
