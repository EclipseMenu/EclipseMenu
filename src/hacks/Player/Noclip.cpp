#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {

    class Noclip : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Noclip", "player.noclip")
                ->setDescription("Disables player death")
                ->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.noclip", false); }
        [[nodiscard]] const char* getId() const override { return "Noclip"; }
    };

    REGISTER_HACK(Noclip)

    class $modify(NoClipPLHook, PlayLayer) {
        struct Fields {
            GameObject* m_anticheatObject = nullptr;
        };
        static void onModify(auto& self) {
            SAFE_PRIORITY("PlayLayer::destroyPlayer");
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            if (!m_fields->m_anticheatObject)
                m_fields->m_anticheatObject = object;

            if (object == m_fields->m_anticheatObject)
                PlayLayer::destroyPlayer(player, object);

            if (!config::get<bool>("player.noclip", false))
                PlayLayer::destroyPlayer(player, object);
        }
    };

}
