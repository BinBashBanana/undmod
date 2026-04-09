#pragma once

#include <Geode/loader/SettingV3.hpp>
#include <matjson.hpp>

#include "RefreshSettingNode.hpp"

namespace horn {

//! @brief Monostate setting value for refresh button.
class RefreshSettingValue : public geode::SettingV3 {
public:
    static geode::Result<std::shared_ptr<geode::SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<RefreshSettingValue>();
        auto root = geode::checkJson(json, "RefreshSettingValue"); // How to use the 2nd argument here?

        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override { return true; }
    bool save(matjson::Value& json) const override { return true; }
    bool isDefaultValue() const override { return true; }
    void reset() override {}

    geode::SettingNodeV3* createNode(float width) override;
};

} // namespace horn
