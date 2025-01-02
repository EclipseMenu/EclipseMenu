#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <utility>

#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Creator {
    class LevelEdit : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.leveledit")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Level Edit"; }
    };

    REGISTER_HACK(LevelEdit)

    class $modify(LevelEditPLHook, PauseLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.leveledit")

        void customSetup() override {
            auto level = utils::get<PlayLayer>()->m_level;
            auto levelType = level->m_levelType;

            level->m_levelType = GJLevelType::Editor;
            PauseLayer::customSetup();
            level->m_levelType = levelType;
        }

        void onEdit(cocos2d::CCObject* sender) {
            auto level = utils::get<PlayLayer>()->m_level;
            auto levelType = level->m_levelType;

            level->m_levelType = GJLevelType::Editor;
            PauseLayer::onEdit(sender);
            level->m_levelType = levelType;
        }
    };

    // 2.207 added "read-only" mode into editor
    // we want to disable it
    #if GEODE_COMP_GD_VERSION > 22070
    class $modify(LevelEditEUIHook, EditorUI) {
        void onSettings(CCObject* sender) {
            auto level = this->m_editorLayer->m_level;
            auto levelType = level->m_levelType;
            level->m_levelType = GJLevelType::Editor;
            EditorUI::onSettings(sender);
            level->m_levelType = levelType;
        }
    };

    class $modify(LevelEditEPLHook, EditorPauseLayer) {
        void customSetup() override {
            auto level = this->m_editorLayer->m_level;
            auto levelType = level->m_levelType;
            level->m_levelType = GJLevelType::Editor;
            EditorPauseLayer::customSetup();
            level->m_levelType = levelType;
        }
    };
    #endif

    // due to some mysterious reason, this will crash in Debug mode
    #ifdef NDEBUG
    class $modify(LevelEditLTHook, LevelTools) {
        ENABLE_SAFE_HOOKS_ALL()

        static bool verifyLevelIntegrity(gd::string levelString, int levelID) {
            if (LevelTools::verifyLevelIntegrity(std::move(levelString), levelID))
                return true;

            return config::get<bool>("creator.leveledit", false);
        }
    };
    #endif
}
