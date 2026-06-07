#include "UNDPopupBody.hpp"

#include <random>
#include <ranges>

#include <Geode/utils/cocos.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/GameStatsManager.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/ui/Notification.hpp>

#include "Manager.hpp"
#include "LevelInfoDownloader.hpp"
#include "EphemeralLevelListLayer.hpp"
#include "misc_nodes/ProgressBar.hpp"
#include "misc_nodes/BoundedMenu.hpp"
#include "misc_nodes/UNDMenuItem.hpp"
#include "misc_nodes/ScrollbarFix.hpp"

namespace horn {

static const std::string difficultyNames[6] = {
    "Auto",
    "Easy",
    "Normal",
    "Hard",
    "Harder",
    "Insane"
};

static const cocos2d::ccColor3B tierColors[7] = {
    { 0x60, 0xff, 0xfe },
    { 0x5f, 0xfb, 0x3a },
    { 0xff, 0xfc, 0x42 },
    { 0xf6, 0x97, 0x2c },
    { 0xf2, 0x0b, 0x1d },
    { 0xff, 0xff, 0xff },
    { 0xff, 0xff, 0xff }
};

static cocos2d::ccColor4F color3Bto4F(cocos2d::ccColor3B rgb, GLuint a) {
    return { rgb.r / 255.0f, rgb.g / 255.0f, rgb.b / 255.0f, a / 255.0f };
}

void UNDPopupBody::setActiveTab(UNDPopupTab tab) {
    m_activeTab = tab;
    if (m_downloadTabActive) {
        tab = UNDPopupTab::None;
    }
    m_tabDownload->setVisible(m_downloadTabActive);
    m_tabAll->setVisible(tab == UNDPopupTab::AllLevels);
    m_tabPacks->setVisible(tab == UNDPopupTab::MapPacks);
    m_tabRandomizer->setVisible(tab == UNDPopupTab::Randomizer);
}

void UNDPopupBody::openPackListFromCache(const std::vector<std::vector<int>>& levelLists, const std::string& name, cocos2d::ccColor3B color, bool sort) {
    auto* list = cocos2d::CCArray::create();
    int indexCounter = 1;
    for (auto idList : levelLists) {
        if (sort) {
            std::sort(idList.begin(), idList.end(), [this](const int& id1, const int& id2) {
                auto level1 = m_gameLevelCache->find(id1);
                auto level2 = m_gameLevelCache->find(id2);
                if (level1 == m_gameLevelCache->end() || level2 == m_gameLevelCache->end()) {
                    return false;
                }
                std::string name1 = level1->second->m_levelName;
                std::string name2 = level2->second->m_levelName;
                return std::lexicographical_compare(
                    name1.begin(), name1.end(),
                    name2.begin(), name2.end(),
                    [](const char& char1, const char& char2) {
                        return tolower(char1) < tolower(char2);
                    }
                );
            });
        }
        for (int levelID : idList) {
            auto level = m_gameLevelCache->find(levelID);
            if (level == m_gameLevelCache->end()) {
                continue;
            }
            // this is fine
            level->second->m_listPosition = indexCounter++;
            list->addObject(level->second.data());
        }
    }

    auto* layer = EphemeralLevelListLayer::create(list, name, color);
    auto* scene = cocos2d::CCScene::create();
    scene->addChild(layer);
    cocos2d::CCDirector::get()->pushScene(cocos2d::CCTransitionFade::create(0.5, scene));
}

void UNDPopupBody::openPackList(const std::vector<int>& levels, const std::vector<std::vector<int>>& levelLists, const std::string& name, cocos2d::ccColor3B color, bool sort) {
    if (levels.size() == 0) {
        geode::Notification::create("No levels!", geode::NotificationIcon::Error, 1.0f)->show();
        return;
    }

    geode::log::info("Opening pack: {}", name);

    // find what level info we need to download
    std::vector<int> toDownload;
    for (int levelID : levels) {
        if (m_gameLevelCache->count(levelID) == 0) {
            toDownload.emplace_back(levelID);
        }
    }

    if (toDownload.size() == 0) {
        // nothing to download
        openPackListFromCache(levelLists, name, color, sort);
    } else {
        // download the level info first
        geode::log::info("Downloading {} level infos...", toDownload.size());
        auto query = fmt::format("{}", fmt::join(toDownload, ","));
        auto* search = GJSearchObject::create(SearchType::MapPackOnClick, query);
        LevelInfoDownloader::download(search, [=, this](cocos2d::CCArray* info) {
            for (auto* level : geode::cocos::CCArrayExt<GJGameLevel*>(info)) {
                m_gameLevelCache->emplace(level->m_levelID, level);
            }
            openPackListFromCache(levelLists, name, color, sort);
        }, [] {
            FLAlertLayer::create(
                "Error",
                "Download failed. Please try again later.",
                "OK"
            )->show();
        });
    }
}

void UNDPopupBody::clickListTrigger(cocos2d::CCObject* sender) {
    auto* target = static_cast<UNDMenuItem*>(sender);
    std::vector<int> levels;
    std::vector<std::vector<int>> levelLists;
    std::string listName;
    cocos2d::ccColor3B listColor;
    bool sort = true;

    if (target->m_isPack) {
        auto& pack = m_sortedPacks[target->m_index1];
        listName = pack.name;
        listColor = pack.color;
        levels = pack.levels;
        levelLists.emplace_back(levels);
        sort = false;
    } else {
        int difficulty = target->m_index1;
        int tier = target->m_index2;
        listColor = tierColors[tier];
        if (tier == 6) {
            // all levels in difficulty
            for (int i = 0; i < 6; i++) {
                levelLists.emplace_back(m_sortedLevels[difficulty][i]);
                levels.insert(levels.end(), m_sortedLevels[difficulty][i].begin(), m_sortedLevels[difficulty][i].end());
            }
            // limit these to 300 levels
            if (levels.size() > 300) {
                geode::Notification::create("Too many levels!", geode::NotificationIcon::Error, 1.0f)->show();
                return;
            }
            listName = fmt::format("All Underrated {}", difficultyNames[difficulty]);
        } else {
            levelLists.emplace_back(m_sortedLevels[difficulty][tier]);
            levels = m_sortedLevels[difficulty][tier];
            listName = fmt::format("Tier {} Underrated {}", tier + 1, difficultyNames[difficulty]);
        }
    }

    openPackList(levels, levelLists, listName, listColor, sort);
}

void UNDPopupBody::clickGoRandomizer(cocos2d::CCObject* sender) {
    bool uncompletedOnly = m_checkboxUncompleted->isToggled();
    bool equalSplit = m_checkboxEqualSplit->isToggled();
    int numToGenerate = m_randomCountSelector->getValue();
    int difficultyMin = m_difficultyMinSelector->getValue();
    int difficultyMax = m_difficultyMaxSelector->getValue();
    int tierMin = m_tierMinSelector->getValue();
    int tierMax = m_tierMaxSelector->getValue();
    std::vector<int> levels;
    std::vector<std::vector<int>> levelLists;
    std::mt19937 rng(std::random_device{}());
    auto isUncompleted = [this](int i) {
        return !m_levelCompletion[i];
    };
    if (equalSplit) {
        int numPerCategory = numToGenerate / ((difficultyMax - difficultyMin + 1) * (tierMax - tierMin + 1));
        for (int difficulty = difficultyMin; difficulty <= difficultyMax; difficulty++) {
            for (int tier = tierMin; tier <= tierMax; tier++) {
                levelLists.emplace_back();
                if (uncompletedOnly) {
                    std::ranges::sample(m_sortedLevels[difficulty][tier - 1] | std::views::filter(isUncompleted), std::back_inserter(levelLists.back()), numPerCategory, rng);
                } else {
                    std::ranges::sample(m_sortedLevels[difficulty][tier - 1], std::back_inserter(levelLists.back()), numPerCategory, rng);
                }
                levels.insert(levels.end(), levelLists.back().begin(), levelLists.back().end());
            }
        }
    } else {
        std::vector<int> pool;
        for (int difficulty = difficultyMin; difficulty <= difficultyMax; difficulty++) {
            for (int tier = tierMin; tier <= tierMax; tier++) {
                pool.insert(pool.end(), m_sortedLevels[difficulty][tier - 1].begin(), m_sortedLevels[difficulty][tier - 1].end());
            }
        }
        if (uncompletedOnly) {
            std::ranges::sample(pool | std::views::filter(isUncompleted), std::back_inserter(levels), numToGenerate, rng);
        } else {
            std::ranges::sample(pool, std::back_inserter(levels), numToGenerate, rng);
        }
        levelLists.emplace_back(levels);
    }

    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    std::string optionsEnc;
    optionsEnc.reserve(5);
    uint32_t options = (difficultyMin << 22) | (difficultyMax << 19) | (tierMin << 16) | (tierMax << 13) | (uncompletedOnly << 12) | (equalSplit << 11) | numToGenerate;
    for (int i = 0; i < 5; i++) {
        optionsEnc.push_back(alphabet[options & 31]);
        options >>= 5;
    }

    std::string randomEnc;
    randomEnc.reserve(8);
    std::uniform_int_distribution<int> randInt(0, 31);
    for (int i = 0; i < 8; i++) {
        randomEnc.push_back(alphabet[randInt(rng)]);
    }

    openPackList(levels, levelLists, fmt::format("UND Random {} {}", optionsEnc, randomEnc), { 80, 190, 255 }, true);
}

void UNDPopupBody::openSheetInBrowser(cocos2d::CCObject* sender) {
    geode::utils::web::openLinkInBrowser("https://docs.google.com/spreadsheets/d/1-Abvx7zXRAqpGFVbdTXpn6g1TRYp9WuZqW2pHWE7Dr4/view");
}

void UNDPopupBody::initMainContent() {
    // spreadsheet link
    auto* spreadsheetLinkBox = cocos2d::CCNode::create();
    spreadsheetLinkBox->setAnchorPoint({ 1.0f, 0.0f });
    spreadsheetLinkBox->setContentSize({ 130.0f, 15.0f });
    spreadsheetLinkBox->setPosition(m_width - 10.0f, 10.0f);
    addChild(spreadsheetLinkBox);
    auto* sheetLinkSprite = cocos2d::CCLabelBMFont::create("Open sheet in browser", "chatFont.fnt");
    sheetLinkSprite->setScale(0.8f);
    sheetLinkSprite->setAnchorPoint({ 0.0f, 0.0f });
    sheetLinkSprite->setPosition(0.0f, 1.5f);
    sheetLinkSprite->setColor({ 0x3e, 0xc3, 0xf0 });
    float sheetLinkWidth = sheetLinkSprite->getScaledContentWidth();
    auto* sheetLinkMenu = cocos2d::CCMenu::create();
    sheetLinkMenu->ignoreAnchorPointForPosition(false);
    sheetLinkMenu->setAnchorPoint({ 0.5f, 0.0f });
    sheetLinkMenu->setContentSize({ sheetLinkWidth, 15.0f });
    sheetLinkMenu->setPosition(65.0f, 0.0f);
    spreadsheetLinkBox->addChild(sheetLinkMenu);
    auto* sheetLinkBtn = cocos2d::CCMenuItem::create(this, menu_selector(UNDPopupBody::openSheetInBrowser));
    sheetLinkBtn->setAnchorPoint({ 0.0f, 0.0f });
    sheetLinkBtn->setContentSize({ sheetLinkWidth, 15.0f });
    sheetLinkMenu->addChild(sheetLinkBtn);
    sheetLinkBtn->addChild(sheetLinkSprite);
    auto* sheetLinkUnderline = ProgressBar::create(sheetLinkWidth, 0.5f, color3Bto4F({ 0x3e, 0xc3, 0xf0 }, 255), { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f);
    sheetLinkUnderline->setAnchorPoint({ 0.5f, 0.0f });
    sheetLinkUnderline->setPosition(65.0f, 2.0f);
    spreadsheetLinkBox->addChild(sheetLinkUnderline);

    // prepare data
    auto* stats = GameStatsManager::sharedState();
    auto* manager = Manager::sharedManager();
    const auto& levels = manager->getCache().getLevels();
    const auto& packs = manager->getPacks();
    int grandTotalLevelsTotal = levels.size();
    int grandTotalLevelsCompleted = 0;

    for (auto& [levelID, info] : levels) {
        m_sortedLevels[static_cast<int>(info.difficulty())][info.tier() - 1].emplace_back(levelID);
        bool completed = stats->hasCompletedOnlineLevel(levelID);
        m_levelCompletion.emplace(levelID, completed);
        grandTotalLevelsCompleted += completed;
    }

    // All Levels tab

    // left side labels
    for (float y : { 216.0f, 95.0f }) {
        for (int i = 0; i < 7; i++) {
            std::string text;
            if (i == 6) {
                text = "Total";
            } else {
                text = fmt::format("Tier {}", i + 1);
            }
            auto* sprite = cocos2d::CCLabelBMFont::create(text.c_str(), "chatFont.fnt");
            sprite->setScale(0.6f);
            sprite->setAnchorPoint({ 0.0f, 0.5f });
            sprite->setPosition(10.0f, y - (i * 15.0f));
            m_tabAll->addChild(sprite);
        }
    }

    // difficulty boxes
    static const cocos2d::CCPoint difficultyBoxPositions[6] = {
        { 40.0f,  226.0f },
        { 120.0f, 226.0f },
        { 200.0f, 226.0f },
        { 40.0f,  105.0f },
        { 120.0f, 105.0f },
        { 200.0f, 105.0f }
    };
    for (int difficulty = 0; difficulty < 6; difficulty++) {
        // box (there are 6, one for each difficulty)
        auto* box = cocos2d::CCMenu::create();
        box->ignoreAnchorPointForPosition(false);
        box->setAnchorPoint({ 0.0f, 1.0f });
        box->setContentSize({ 65.0f, 110.0f });
        box->setPosition(difficultyBoxPositions[difficulty]);
        m_tabAll->addChild(box);

        // difficulty label above each box
        auto* difficultyLabel = cocos2d::CCLabelBMFont::create(difficultyNames[difficulty].c_str(), "chatFont.fnt");
        difficultyLabel->setScale(0.8f);
        difficultyLabel->setAnchorPoint({ 0.5f, 0.0f });
        difficultyLabel->setPosition({ difficultyBoxPositions[difficulty].x + 32.5f, difficultyBoxPositions[difficulty].y - 3.5f });
        m_tabAll->addChild(difficultyLabel);

        // for every tier...
        int totalLevelsTotal = 0;
        int totalLevelsCompleted = 0;
        for (int tier = 0; tier < 7; tier++) {
            auto* clickTarget = UNDMenuItem::create(this, menu_selector(UNDPopupBody::clickListTrigger));
            clickTarget->setAnchorPoint({ 0.0f, 0.5f });
            clickTarget->setContentSize({ 65.0f, 15.0f });
            clickTarget->setPosition(0.0f, 100.0f - (tier * 15.0f));
            box->addChild(clickTarget);

            clickTarget->m_isPack = false;
            clickTarget->m_index1 = difficulty;
            clickTarget->m_index2 = tier;
            clickTarget->m_tintSelected = true;

            // level count calculation
            int levelsTotal;
            int levelsCompleted;
            if (tier < 6) {
                auto& levels = m_sortedLevels[difficulty][tier];
                levelsTotal = levels.size();
                levelsCompleted = 0;
                for (int levelID : levels) {
                    levelsCompleted += m_levelCompletion[levelID];
                }
                totalLevelsTotal += levelsTotal;
                totalLevelsCompleted += levelsCompleted;
            } else {
                levelsTotal = totalLevelsTotal;
                levelsCompleted = totalLevelsCompleted;
            }

            // text sprites
            std::string textLeft;
            std::string textRight;
            if (levelsTotal) {
                textLeft = fmt::format("{}/{}", levelsCompleted, levelsTotal);
                textRight = fmt::format("{}%", levelsCompleted * 100 / levelsTotal); // floor

                auto* spriteRight = cocos2d::CCLabelBMFont::create(textRight.c_str(), "chatFont.fnt");
                spriteRight->setScale(0.5f);
                spriteRight->setAnchorPoint({ 1.0f, 1.0f });
                spriteRight->setPosition(65.0f, 14.0f);
                clickTarget->addChild(spriteRight);
            } else {
                textLeft = "N/A";
            }
            auto* spriteLeft = cocos2d::CCLabelBMFont::create(textLeft.c_str(), "chatFont.fnt");
            spriteLeft->setScale(0.5f);
            spriteLeft->setAnchorPoint({ 0.0f, 1.0f });
            spriteLeft->setPosition(0.0f, 14.0f);
            clickTarget->addChild(spriteLeft);

            // progress bar
            auto* bar = ProgressBar::create(65.0f, 2.5f, color3Bto4F({ 90, 90, 90 }, 255), color3Bto4F(tierColors[tier], 255),
                levelsCompleted ? static_cast<float>(levelsCompleted) / levelsTotal : 0.0f
            );
            bar->setAnchorPoint({ 0.0f, 0.0f });
            bar->setPosition(0.0f, 2.0f);
            clickTarget->addChild(bar);
        }
    }

    // the big bar
    {
        auto* totalBox = cocos2d::CCNode::create();
        totalBox->setAnchorPoint({ 1.0f, 1.0f });
        totalBox->setContentSize({ 130.0f, 35.0f });
        totalBox->setPosition(m_width - 10.0f, m_height);
        m_tabAll->addChild(totalBox);

        auto* totalLabel = cocos2d::CCLabelBMFont::create("Total list levels complete:", "chatFont.fnt");
        totalLabel->setScale(0.8f);
        totalLabel->setAnchorPoint({ 0.5f, 1.0f });
        totalLabel->setPosition({ 65.0f, 35.0f });
        totalBox->addChild(totalLabel);

        auto textLeft = fmt::format("{}/{}", grandTotalLevelsCompleted, grandTotalLevelsTotal);
        auto* spriteLeft = cocos2d::CCLabelBMFont::create(textLeft.c_str(), "chatFont.fnt");
        spriteLeft->setScale(0.6f);
        spriteLeft->setAnchorPoint({ 0.0f, 1.0f });
        spriteLeft->setPosition(0.0f, 17.0f);
        totalBox->addChild(spriteLeft);

        auto textRight = fmt::format("{}%", grandTotalLevelsCompleted * 100 / grandTotalLevelsTotal); // floor
        auto* spriteRight = cocos2d::CCLabelBMFont::create(textRight.c_str(), "chatFont.fnt");
        spriteRight->setScale(0.6f);
        spriteRight->setAnchorPoint({ 1.0f, 1.0f });
        spriteRight->setPosition(130.0f, 17.0f);
        totalBox->addChild(spriteRight);

        auto* bar = ProgressBar::create(130.0f, 5.0f, color3Bto4F({ 90, 90, 90 }, 255), color3Bto4F({ 255, 255, 255 }, 255),
            static_cast<float>(grandTotalLevelsCompleted) / grandTotalLevelsTotal
        );
        bar->setAnchorPoint({ 0.0f, 1.0f });
        bar->setPosition(0.0f, 5.0f);
        totalBox->addChild(bar);
    }

    // Map Packs tab

    int totalMapPackLevelsTotal = 0;
    int totalMapPackLevelsCompleted = 0;
    int mapPacksTotal = 0;
    int mapPacksCompleted = 0;

    {
        auto* background = cocos2d::CCLayerColor::create({ 67, 67, 67, 255 }, 200.0f, m_height);
        background->setAnchorPoint({ 0.0f, 0.0f });
        background->setContentSize({ 200.0f, m_height });
        background->setPosition(30.0f, 0.0f);
        m_tabPacks->addChild(background);

        m_scrollLayer = geode::ScrollLayer::create({ 200.0f, m_height }, true, true);
        m_scrollLayer->setStealingTouches(true);
        m_scrollLayer->setAnchorPoint({ 0.0f, 0.0f });
        m_scrollLayer->setPosition(30.0f, 0.0f);
        m_tabPacks->addChild(m_scrollLayer);

        std::vector<int> packLevelIDs;
        int packIndex = 0;
        bool bgType = false; // thanks geode for this trick

        for (auto& [category, contents] : packs) {
            auto colorRes = contents.get<std::vector<GLubyte>>("color");
            cocos2d::ccColor3B color = { 255, 255, 255 };
            if (colorRes.isOk()) {
                auto colorTmp = colorRes.unwrap();
                color.r = colorTmp[0];
                color.g = colorTmp[1];
                color.b = colorTmp[2];
            }
            {
                // category header
                auto* label = cocos2d::CCLabelBMFont::create(category.c_str(), "bigFont.fnt");
                label->setScale(0.4f);
                label->setColor(color);
                label->setAnchorPoint({ 0.0f, 0.5f });
                label->setPosition(7.0f, 10.0f);

                cocos2d::ccColor4B bg(0, 0, 0, bgType ? 60 : 20);
                bgType = !bgType;
                auto* layer = cocos2d::CCLayerColor::create(bg, 200.0f, 20.0f);
                layer->addChild(label);
                m_scrollLayer->m_contentLayer->addChild(layer);
            }
            for (auto& [pack, packContents] : contents["packs"]) {
                auto packLevels = packContents.as<std::vector<int>>();
                if (packLevels.isOk()) {
                    auto levels = packLevels.unwrap();
                    int levelsCompleted = 0;
                    int levelsTotal = levels.size();
                    for (int levelID : levels) {
                        auto completedIter = m_levelCompletion.find(levelID);
                        bool completed;
                        if (completedIter == m_levelCompletion.end()) {
                            completed = stats->hasCompletedOnlineLevel(levelID);
                            m_levelCompletion.emplace(levelID, completed);
                        } else {
                            completed = completedIter->second;
                        }
                        levelsCompleted += completed;
                        if (std::find(packLevelIDs.begin(), packLevelIDs.end(), levelID) == packLevelIDs.end()) {
                            packLevelIDs.emplace_back(levelID);
                            totalMapPackLevelsTotal++;
                            totalMapPackLevelsCompleted += completed;
                        }
                    }
                    m_sortedPacks.emplace_back(pack, color, levels);
                    if (levelsCompleted == levelsTotal) {
                        mapPacksCompleted++;
                    }

                    // each pack
                    auto* label = cocos2d::CCLabelBMFont::create(pack.c_str(), "chatFont.fnt");
                    label->setScale(0.8f);
                    label->setAnchorPoint({ 0.0f, 0.0f });
                    label->setPosition(15.0f, 15.0f);

                    auto completedText = fmt::format("{}/{}", levelsCompleted, levelsTotal);
                    auto* completedSprite = cocos2d::CCLabelBMFont::create(completedText.c_str(), "chatFont.fnt");
                    completedSprite->setScale(0.8f);
                    completedSprite->setAnchorPoint({ 1.0f, 0.0f });
                    completedSprite->setPosition(185.0f, 15.0f);

                    auto* bar = ProgressBar::create(170.0f, 2.5f, color3Bto4F({ 90, 90, 90 }, 255), color3Bto4F(color, 255),
                        static_cast<float>(levelsCompleted) / levelsTotal
                    );
                    bar->setAnchorPoint({ 0.0f, 0.0f });
                    bar->setPosition(15.0f, 11.0f);

                    auto* clickTarget = UNDMenuItem::create(this, menu_selector(UNDPopupBody::clickListTrigger));
                    clickTarget->setAnchorPoint({ 0.0f, 0.0f });
                    clickTarget->setContentSize({ 200.0f, 40.0f });
                    clickTarget->setPosition(0.0f, 0.0f);
                    clickTarget->addChild(label);
                    clickTarget->addChild(completedSprite);
                    clickTarget->addChild(bar);

                    clickTarget->m_isPack = true;
                    clickTarget->m_index1 = packIndex++;
                    clickTarget->m_tintSelected = true;

                    auto* menu = BoundedMenu::create();
                    menu->setTouchRectNode(m_scrollLayer);
                    menu->setContentSize({ 200.0f, 40.0f });
                    menu->setPosition(0.0f, 0.0f);
                    menu->addChild(clickTarget);

                    cocos2d::ccColor4B bg(0, 0, 0, bgType ? 60 : 20);
                    bgType = !bgType;
                    auto* layer = cocos2d::CCLayerColor::create(bg, 200.0f, 40.0f);
                    layer->addChild(menu);
                    m_scrollLayer->m_contentLayer->addChild(layer);
                }
            }
        }

        mapPacksTotal = packIndex;

        m_scrollLayer->m_contentLayer->setLayout(geode::ScrollLayer::createDefaultListLayout(0.0f));
        if (m_initialState.scrollHeight != -1.0f) {
            m_scrollLayer->m_contentLayer->setPositionY(m_initialState.scrollHeight);
        } else {
            m_scrollLayer->scrollToTop();
        }

        auto* scrollBar = ScrollbarFix::create(m_scrollLayer);
        scrollBar->setAnchorPoint({ 0.0f, 0.0f });
        scrollBar->setContentSize({ 8.0f, m_height });
        scrollBar->setPosition(232.0f, 0.0f);
        m_tabPacks->addChild(scrollBar);
    }

    // map packs tab: total level completion bar
    {
        auto* totalBox = cocos2d::CCNode::create();
        totalBox->setAnchorPoint({ 1.0f, 1.0f });
        totalBox->setContentSize({ 130.0f, 35.0f });
        totalBox->setPosition(m_width - 10.0f, m_height);
        m_tabPacks->addChild(totalBox);

        auto* totalLabel = cocos2d::CCLabelBMFont::create("Total pack levels complete:", "chatFont.fnt");
        totalLabel->setScale(0.8f);
        totalLabel->setAnchorPoint({ 0.5f, 1.0f });
        totalLabel->setPosition({ 65.0f, 35.0f });
        totalBox->addChild(totalLabel);

        auto textLeft = fmt::format("{}/{}", totalMapPackLevelsCompleted, totalMapPackLevelsTotal);
        auto* spriteLeft = cocos2d::CCLabelBMFont::create(textLeft.c_str(), "chatFont.fnt");
        spriteLeft->setScale(0.6f);
        spriteLeft->setAnchorPoint({ 0.0f, 1.0f });
        spriteLeft->setPosition(0.0f, 17.0f);
        totalBox->addChild(spriteLeft);

        auto textRight = fmt::format("{}%", totalMapPackLevelsCompleted * 100 / totalMapPackLevelsTotal); // floor
        auto* spriteRight = cocos2d::CCLabelBMFont::create(textRight.c_str(), "chatFont.fnt");
        spriteRight->setScale(0.6f);
        spriteRight->setAnchorPoint({ 1.0f, 1.0f });
        spriteRight->setPosition(130.0f, 17.0f);
        totalBox->addChild(spriteRight);

        auto* bar = ProgressBar::create(130.0f, 5.0f, color3Bto4F({ 90, 90, 90 }, 255), color3Bto4F({ 255, 255, 255 }, 255),
            static_cast<float>(totalMapPackLevelsCompleted) / totalMapPackLevelsTotal
        );
        bar->setAnchorPoint({ 0.0f, 1.0f });
        bar->setPosition(0.0f, 5.0f);
        totalBox->addChild(bar);
    }

    // map packs tab: total pack completion bar
    {
        auto* totalBox = cocos2d::CCNode::create();
        totalBox->setAnchorPoint({ 1.0f, 1.0f });
        totalBox->setContentSize({ 130.0f, 35.0f });
        totalBox->setPosition(m_width - 10.0f, m_height - 50.0f);
        m_tabPacks->addChild(totalBox);

        auto* totalLabel = cocos2d::CCLabelBMFont::create("Total packs complete:", "chatFont.fnt");
        totalLabel->setScale(0.8f);
        totalLabel->setAnchorPoint({ 0.5f, 1.0f });
        totalLabel->setPosition({ 65.0f, 35.0f });
        totalBox->addChild(totalLabel);

        auto textLeft = fmt::format("{}/{}", mapPacksCompleted, mapPacksTotal);
        auto* spriteLeft = cocos2d::CCLabelBMFont::create(textLeft.c_str(), "chatFont.fnt");
        spriteLeft->setScale(0.6f);
        spriteLeft->setAnchorPoint({ 0.0f, 1.0f });
        spriteLeft->setPosition(0.0f, 17.0f);
        totalBox->addChild(spriteLeft);

        auto textRight = fmt::format("{}%", mapPacksCompleted * 100 / mapPacksTotal); // floor
        auto* spriteRight = cocos2d::CCLabelBMFont::create(textRight.c_str(), "chatFont.fnt");
        spriteRight->setScale(0.6f);
        spriteRight->setAnchorPoint({ 1.0f, 1.0f });
        spriteRight->setPosition(130.0f, 17.0f);
        totalBox->addChild(spriteRight);

        auto* bar = ProgressBar::create(130.0f, 5.0f, color3Bto4F({ 90, 90, 90 }, 255), color3Bto4F({ 255, 255, 255 }, 255),
            static_cast<float>(mapPacksCompleted) / mapPacksTotal
        );
        bar->setAnchorPoint({ 0.0f, 1.0f });
        bar->setPosition(0.0f, 5.0f);
        totalBox->addChild(bar);
    }

    // Randomizer tab

    {
        auto* difficultyMinLabel = cocos2d::CCLabelBMFont::create("Difficulty min.", "bigFont.fnt");
        difficultyMinLabel->setScale(0.4f);
        difficultyMinLabel->setPosition(100.0f, 215.0f);
        m_tabRandomizer->addChild(difficultyMinLabel);
        m_difficultyMinSelector = LRSelectorEnum::create(difficultyNames, m_initialState.difficultyMin, false, [this](int newValue) {
            if (newValue > m_difficultyMaxSelector->getValue()) {
                m_difficultyMaxSelector->setValue(newValue, false);
            }
            updateRandomizerCountStep(m_checkboxEqualSplit->isToggled());
        }, 140.0f);
        m_difficultyMinSelector->setPosition(100.0f, 190.0f);
        m_tabRandomizer->addChild(m_difficultyMinSelector);

        auto* difficultyMaxLabel = cocos2d::CCLabelBMFont::create("Difficulty max.", "bigFont.fnt");
        difficultyMaxLabel->setScale(0.4f);
        difficultyMaxLabel->setPosition(100.0f, 155.0f);
        m_tabRandomizer->addChild(difficultyMaxLabel);
        m_difficultyMaxSelector = LRSelectorEnum::create(difficultyNames, m_initialState.difficultyMax, false, [this](int newValue) {
            if (newValue < m_difficultyMinSelector->getValue()) {
                m_difficultyMinSelector->setValue(newValue, false);
            }
            updateRandomizerCountStep(m_checkboxEqualSplit->isToggled());
        }, 140.0f);
        m_difficultyMaxSelector->setPosition(100.0f, 130.0f);
        m_tabRandomizer->addChild(m_difficultyMaxSelector);

        auto* tierMinLabel = cocos2d::CCLabelBMFont::create("Tier min.", "bigFont.fnt");
        tierMinLabel->setScale(0.4f);
        tierMinLabel->setPosition(50.0f, 95.0f);
        m_tabRandomizer->addChild(tierMinLabel);
        m_tierMinSelector = LRSelectorInt::create(1, 5, m_initialState.tierMin, 1, false, false, false, [this](int newValue) {
            if (newValue > m_tierMaxSelector->getValue()) {
                m_tierMaxSelector->setValue(newValue, false);
            }
            updateRandomizerCountStep(m_checkboxEqualSplit->isToggled());
        });
        m_tierMinSelector->setPosition(50.0f, 70.0f);
        m_tabRandomizer->addChild(m_tierMinSelector);

        auto* tierMaxLabel = cocos2d::CCLabelBMFont::create("Tier max.", "bigFont.fnt");
        tierMaxLabel->setScale(0.4f);
        tierMaxLabel->setPosition(150.0f, 95.0f);
        m_tabRandomizer->addChild(tierMaxLabel);
        m_tierMaxSelector = LRSelectorInt::create(1, 5, m_initialState.tierMax, 1, false, false, false, [this](int newValue) {
            if (newValue < m_tierMinSelector->getValue()) {
                m_tierMinSelector->setValue(newValue, false);
            }
            updateRandomizerCountStep(m_checkboxEqualSplit->isToggled());
        });
        m_tierMaxSelector->setPosition(150.0f, 70.0f);
        m_tabRandomizer->addChild(m_tierMaxSelector);

        auto* randomCountLabel = cocos2d::CCLabelBMFont::create("Max levels to generate", "bigFont.fnt");
        randomCountLabel->setScale(0.4f);
        randomCountLabel->setPosition(300.0f, 95.0f);
        m_tabRandomizer->addChild(randomCountLabel);
        m_randomCountSelector = LRSelectorInt::create(1, 100, m_initialState.randomCount, 1, true, true, true, nullptr, 60.0f);
        m_randomCountSelector->setPosition(300.0f, 70.0f);
        m_tabRandomizer->addChild(m_randomCountSelector);

        auto* checkboxMenu = cocos2d::CCMenu::create();
        checkboxMenu->setContentSize({ 30.0f, 90.0f });
        checkboxMenu->setPosition(250.0f, 115.0f);
        m_tabRandomizer->addChild(checkboxMenu);

        m_checkboxUncompleted = CCMenuItemToggler::createWithStandardSprites(this, nullptr, 1.0f);
        m_checkboxUncompleted->toggle(m_initialState.uncompletedOnly);
        m_checkboxUncompleted->setSizeMult(1.2f);
        m_checkboxUncompleted->setPosition(15.0f, 75.0f);
        checkboxMenu->addChild(m_checkboxUncompleted);
        auto* labelUncompleted = cocos2d::CCLabelBMFont::create("Uncompleted only", "bigFont.fnt");
        labelUncompleted->setScale(0.4f);
        labelUncompleted->setAnchorPoint({ 0.0f, 0.5f });
        labelUncompleted->setPosition(290.0f, 190.0f);
        m_tabRandomizer->addChild(labelUncompleted);

        m_checkboxEqualSplit = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(UNDPopupBody::clickEqualSplit), 1.0f);
        m_checkboxEqualSplit->toggle(m_initialState.equalSplit);
        m_checkboxEqualSplit->setSizeMult(1.2f);
        m_checkboxEqualSplit->setPosition(15.0f, 15.0f);
        checkboxMenu->addChild(m_checkboxEqualSplit);
        updateRandomizerCountStep(m_initialState.equalSplit);
        auto* labelEqualSplit = cocos2d::CCLabelBMFont::create("Equal split", "bigFont.fnt");
        labelEqualSplit->setScale(0.4f);
        labelEqualSplit->setAnchorPoint({ 0.0f, 0.5f });
        labelEqualSplit->setPosition(290.0f, 130.0f);
        m_tabRandomizer->addChild(labelEqualSplit);

        auto* goMenu = cocos2d::CCMenu::create();
        goMenu->ignoreAnchorPointForPosition(false);
        goMenu->setPosition(210.0f, 22.0f);
        m_tabRandomizer->addChild(goMenu);

        auto* goSprite = ButtonSprite::create("Go", 0.0f, false, "goldFont.fnt", "GJ_button_01.png", 0.0f, 0.8f);
        goSprite->setScale(1.1f);
        auto* goButton = CCMenuItemSpriteExtra::create(
            goSprite,
            this,
            menu_selector(UNDPopupBody::clickGoRandomizer)
        );
        goButton->setSizeMult(1.2f);
        goButton->setPosition(goButton->getContentSize() / 2.0f);
        goMenu->setContentSize(goButton->getContentSize());
        goMenu->addChild(goButton);
    }
}

void UNDPopupBody::updateRandomizerCountStep(bool enabled) {
    m_randomCountSelector->setStep(
        enabled ? (m_difficultyMaxSelector->getValue() - m_difficultyMinSelector->getValue() + 1) * (m_tierMaxSelector->getValue() - m_tierMinSelector->getValue() + 1) : 1,
        true
    );
}

void UNDPopupBody::clickEqualSplit(cocos2d::CCObject* sender) {
    auto* target = static_cast<CCMenuItemToggler*>(sender);
    updateRandomizerCountStep(!target->isToggled());
}

UNDPopupBodyState UNDPopupBody::getState() const {
    UNDPopupBodyState state{};
    if (m_scrollLayer) {
        state.scrollHeight = m_scrollLayer->m_contentLayer->getPositionY();
    }
    if (m_difficultyMinSelector) {
        state.difficultyMin = m_difficultyMinSelector->getValue();
        state.difficultyMax = m_difficultyMaxSelector->getValue();
        state.tierMin = m_tierMinSelector->getValue();
        state.tierMax = m_tierMaxSelector->getValue();
        state.randomCount = m_randomCountSelector->getValue();
        state.uncompletedOnly = m_checkboxUncompleted->isToggled();
        state.equalSplit = m_checkboxEqualSplit->isToggled();
    }
    return state;
}

void UNDPopupBody::clickRefresh(cocos2d::CCObject* sender) {
    m_downloadButton->setVisible(false);
    m_downloadSpinner->setVisible(true);

    Manager::sharedManager()->download(
        [this] {
            m_downloadSpinner->setVisible(false);
            m_downloadButton->setVisible(true);

            m_downloadTabActive = false;
            setActiveTab(m_activeTab);
            initMainContent();
        },
        [this](std::string err) {
            m_downloadSpinner->setVisible(false);
            m_downloadButton->setVisible(true);

            FLAlertLayer::create(
                "Error",
                fmt::format("Download failed. Please try again later.\nMessage: {}", err),
                "OK"
            )->show();
        }
    );
}

bool UNDPopupBody::init(float width, float height, const UNDPopupBodyState& state) {
    if (!cocos2d::CCNode::init()) {
        return false;
    }

    setContentSize({ width, height });
    m_width = width;
    m_height = height;
    m_initialState = state;

    // tab roots
    m_tabDownload = cocos2d::CCNode::create();
    m_tabDownload->setAnchorPoint({ 0.0f, 0.0f });
    m_tabDownload->setContentSize({ width, height });
    addChild(m_tabDownload);
    m_tabAll = cocos2d::CCNode::create();
    m_tabAll->setAnchorPoint({ 0.0f, 0.0f });
    m_tabAll->setContentSize({ width, height });
    addChild(m_tabAll);
    m_tabPacks = cocos2d::CCNode::create();
    m_tabPacks->setAnchorPoint({ 0.0f, 0.0f });
    m_tabPacks->setContentSize({ width, height });
    addChild(m_tabPacks);
    m_tabRandomizer = cocos2d::CCNode::create();
    m_tabRandomizer->setAnchorPoint({ 0.0f, 0.0f });
    m_tabRandomizer->setContentSize({ width, height });
    addChild(m_tabRandomizer);

    // Download tab for if the sheet data is somehow missing
    {
        auto* textSprite = cocos2d::CCLabelBMFont::create("Sheet data is missing!", "bigFont.fnt");
        textSprite->setScale(0.8f);
        textSprite->setPosition(width / 2.0f, height / 2.0f + 15.0f);
        m_tabDownload->addChild(textSprite);
        auto* textSprite2 = cocos2d::CCLabelBMFont::create("Click the button to reload data.", "bigFont.fnt");
        textSprite2->setScale(0.4f);
        textSprite2->setAnchorPoint({ 0.0f, 0.5f });
        textSprite2->setPosition((width - textSprite->getScaledContentWidth()) / 2.0f + 1.0f, height / 2.0f - 15.0f);
        m_tabDownload->addChild(textSprite2);
        cocos2d::CCPoint pos = { textSprite2->getPositionX() + textSprite2->getScaledContentWidth() + 20.0f, height / 2.0f - 15.0f };
        auto* menu = cocos2d::CCMenu::create();
        menu->ignoreAnchorPointForPosition(false);
        menu->setPosition(pos);
        menu->setContentSize({ 20.0f, 20.0f });
        m_tabDownload->addChild(menu);
        auto* buttonSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        buttonSprite->setScale(0.5f);
        m_downloadButton = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(UNDPopupBody::clickRefresh)
        );
        m_downloadButton->setPosition({ 10.0f, 10.0f });
        menu->addChild(m_downloadButton);
        m_downloadSpinner = geode::LoadingSpinner::create(20.0f);
        m_downloadSpinner->setPosition(pos);
        m_downloadSpinner->setVisible(false);
        m_tabDownload->addChild(m_downloadSpinner);
    }

    if (Manager::sharedManager()->getCache().getLevels().size() == 0) {
        m_downloadTabActive = true;
    } else {
        initMainContent();
    }
    setActiveTab(UNDPopupTab::AllLevels);

    return true;
}

}
