#include "RefreshSettingValue.hpp"

#include <Geode/loader/Mod.hpp>

namespace horn {

geode::SettingNodeV3* RefreshSettingValue::createNode(float width) {
    return RefreshSettingNode::create(
        std::static_pointer_cast<RefreshSettingValue>(shared_from_this()),
        width
    );
}

}

$execute {
    (void)geode::Mod::get()->registerCustomSettingType("refreshbutton", &horn::RefreshSettingValue::parse);
}
