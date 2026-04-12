#include "Cache.hpp"

#include <Geode/loader/Log.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/utils/general.hpp>

#include "CSV.hpp"

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
    auto ts = json["timestamp"].asInt();
    if (ts.isErr()) {
        return;
    }
    m_timestamp = ts.unwrap();

    for (auto& [k, v] : json["levels"]) {
        int levelID = geode::utils::numFromString<int>(k).unwrapOrDefault();
        auto info = horn::LevelInfo(v);

        m_levels[levelID] = info;
    }
}

matjson::Value Cache::json() const {
    matjson::Value levels;
    for (auto [levelID, info] : m_levels) {
        std::string k = std::to_string(levelID);

        levels[k] = info;
    }

    matjson::Value res;
    res["timestamp"] = m_timestamp;
    res["levels"] = levels;

    return res;
}

std::optional<LevelInfo> Cache::getLevelInfo(int levelID) {
    if (m_levels.count(levelID) == 0) {
        return std::nullopt;
    }

    return m_levels[levelID];
}

} // namespace horn
