#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/NumberInputLayer.hpp>

namespace eclipse::hacks::Bypass {

    class CopyBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Copy Bypass", "bypass.copybypass")->setDescription("Shows the copy password for copyable levels");
        }

        [[nodiscard]] const char* getId() const override { return "Level Copy Bypass"; }
    };

    REGISTER_HACK(CopyBypass)

    class $modify(NumberInputLayer) {
        bool init() override {
            if (!NumberInputLayer::init()) return false;

            if (auto lil = geode::cocos::getChildOfType<LevelInfoLayer>(cocos2d::CCScene::get(), 0)) {
                int pass = lil->m_level->m_password.value();
                if (config::get<bool>("bypass.copybypass", false)) geode::cocos::getChildOfType<cocos2d::CCLabelBMFont>(m_mainLayer, 0)->setString(fmt::format("Password: {}", pass).c_str());
            }

            return true;
        }
    };

}
