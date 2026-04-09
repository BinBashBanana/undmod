#include "LevelInfo.hpp"
#include <Geode/utils/general.hpp>

namespace horn {

LevelInfo::LevelInfo(std::vector<std::string> const& vec) {
    m_tier = geode::utils::numFromString<int>(vec[1]).unwrapOrDefault();
    m_skillset = vec[2];
    m_description = vec[3];
}

LevelInfo::LevelInfo(matjson::Value const& json) {
    m_tier = json["tier"].asInt().unwrapOrDefault();
    m_skillset = json["skillset"].asString().unwrapOrDefault();
    m_description = json["description"].asString().unwrapOrDefault();
}

matjson::Value LevelInfo::json() const {
    matjson::Value json;

    json["tier"] = m_tier;
    json["skillset"] = m_skillset;
    json["description"] = m_description;

    return json;
}

} // namespace horn

