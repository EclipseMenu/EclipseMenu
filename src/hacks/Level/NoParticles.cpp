#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    void onHideParticles(bool state) {
        if (!state) return;

        bool customParticlesEnabled = !config::get<bool>("level.noparticles.nocustomparticles", false);
        bool miscParticlesEnabled = !config::get<bool>("level.noparticles.nomiscparticles", false);

        if (customParticlesEnabled && miscParticlesEnabled)
            config::set("level.noparticles", false);

        auto* gjbgl = GJBaseGameLayer::get();

        if (!gjbgl) return;

        for (const auto& [name, array] : geode::cocos::CCDictionaryExt<gd::string, cocos2d::CCArray*>{ gjbgl->m_particlesDict }) {
            for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{ array }) {
                particle->setVisible(miscParticlesEnabled);
            }
        }

        for (const auto& [name, array] : geode::cocos::CCDictionaryExt<gd::string, cocos2d::CCArray*>{ gjbgl->m_claimedParticles }) {
            for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{ array }) {
                particle->setVisible(customParticlesEnabled);
            }
        }

        for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{ gjbgl->m_unclaimedParticles }) {
            particle->setVisible(customParticlesEnabled);
        }

        for (const auto& [name, array] : geode::cocos::CCDictionaryExt<gd::string, cocos2d::CCArray*>{ gjbgl->m_customParticles }) {
            for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{ array }) {
                particle->setVisible(customParticlesEnabled);
            }
        }
    }

    class NoParticles : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            config::setIfEmpty("level.noparticles", false);
            config::setIfEmpty("level.noparticles.nomiscparticles", true);
            config::setIfEmpty("level.noparticles.nocustomparticles", false);

            tab->addToggle("No Particles", "level.noparticles")
                ->handleKeybinds()
                ->setDescription("Hides portal, coin, custom, etc particles in levels.")
                ->callback(onHideParticles)
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addToggle("No Misc. Particles", "level.noparticles.nomiscparticles")
                        ->setDescription("Includes portal, dash orb, coin, end wall particles, etc...");
                    options->addToggle("No Custom Particles", "level.noparticles.nocustomparticles")
                        ->setDescription("Includes particles created by the level author.");
                });
        }

        void update() override {
            onHideParticles(config::get<bool>("level.noparticles", false));
        }

        [[nodiscard]] const char* getId() const override { return "No Particles"; }
    };

    REGISTER_HACK(NoParticles)

    class $modify(NoParticlesBGLHook, GJBaseGameLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("GJBaseGameLayer::spawnParticle");
        }

        cocos2d::CCParticleSystemQuad* spawnParticle(char const* plist, int zOrder, cocos2d::tCCPositionType positionType, cocos2d::CCPoint position) {
            if (config::get<bool>("level.noparticles", false) && config::get<bool>("level.noparticles.nomiscparticles", false))
                return nullptr;

            return GJBaseGameLayer::spawnParticle(plist, zOrder, positionType, position);
        }
    };
}
