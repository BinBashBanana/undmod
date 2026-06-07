#include "LevelInfo.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/loader/Log.hpp>

namespace horn {

LevelInfo::LevelInfo(std::vector<std::string> const& vec) {
    m_tier = geode::utils::numFromString<int>(vec[1]).unwrapOrDefault();
    m_skillset = vec[3];
    m_description = vec[4];
    if (vec[2] == "Insane") {
        m_difficulty = GJDifficulty::Insane;
    } else if (vec[2] == "Harder") {
        m_difficulty = GJDifficulty::Harder;
    } else if (vec[2] == "Hard") {
        m_difficulty = GJDifficulty::Hard;
    } else if (vec[2] == "Normal") {
        m_difficulty = GJDifficulty::Normal;
    } else if (vec[2] == "Easy") {
        m_difficulty = GJDifficulty::Easy;
    } else if (vec[2] == "Auto") {
        m_difficulty = GJDifficulty::Auto;
    } else {
        m_difficulty = GJDifficulty::NA;
        geode::log::warn("Unknown difficulty '{}' for level ID {}", vec[2], vec[0]);
    }
}

LevelInfo::LevelInfo(matjson::Value const& json) {
    m_tier = json["tier"].asInt().unwrapOrDefault();
    m_difficulty = static_cast<GJDifficulty>(json["difficulty"].asInt().unwrapOrDefault());
    m_skillset = json["skillset"].asString().unwrapOrDefault();
    m_description = json["description"].asString().unwrapOrDefault();
}

matjson::Value LevelInfo::json() const {
    matjson::Value json;

    json["tier"] = m_tier;
    json["difficulty"] = static_cast<int>(m_difficulty);
    json["skillset"] = m_skillset;
    json["description"] = m_description;

    return json;
}

} // namespace horn

