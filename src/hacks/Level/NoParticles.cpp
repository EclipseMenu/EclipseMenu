#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {
    void onHideParticles(bool state) {
        if (!state) return;

        bool customParticlesEnabled = !config::get<bool>("level.noparticles.nocustomparticles", false);
        bool miscParticlesEnabled = !config::get<bool>("level.noparticles.nomiscparticles", false);

        if (customParticlesEnabled && miscParticlesEnabled)
            config::set("level.noparticles", false);

        auto* gjbgl = utils::get<GJBaseGameLayer>();

        if (!gjbgl) return;

        for (const auto& [name, array] : geode::cocos::CCDictionaryExt<gd::string, cocos2d::CCArray*>{gjbgl->m_particlesDict}) {
            for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{array}) {
                particle->setVisible(miscParticlesEnabled);
            }
        }

        for (const auto& [name, array] : geode::cocos::CCDictionaryExt<gd::string, cocos2d::CCArray*>{gjbgl->m_claimedParticles}) {
            for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{array}) {
                particle->setVisible(customParticlesEnabled);
            }
        }

        for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{gjbgl->m_unclaimedParticles}) {
            particle->setVisible(customParticlesEnabled);
        }

        for (const auto& [name, array] : geode::cocos::CCDictionaryExt<gd::string, cocos2d::CCArray*>{gjbgl->m_customParticles}) {
            for (const auto& particle : geode::cocos::CCArrayExt<cocos2d::CCParticleSystemQuad*>{array}) {
                particle->setVisible(customParticlesEnabled);
            }
        }
    }

    class $hack(NoParticles) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.noparticles", false);
            config::setIfEmpty("level.noparticles.nomiscparticles", true);
            config::setIfEmpty("level.noparticles.nocustomparticles", false);

            tab->addToggle("level.noparticles")->handleKeybinds()->setDescription()
               ->callback(onHideParticles)
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addToggle("level.noparticles.nomiscparticles")->setDescription();
                   options->addToggle("level.noparticles.nocustomparticles")->setDescription();
               });
        }

        void update() override {
            onHideParticles(config::get<bool>("level.noparticles", false));
        }

        [[nodiscard]] const char* getId() const override { return "No Particles"; }
    };

    REGISTER_HACK(NoParticles)

    class $modify(NoParticlesBGLHook, GJBaseGameLayer) {
        ENABLE_SAFE_HOOKS_ALL()

        cocos2d::CCParticleSystemQuad* spawnParticle(char const* plist, int zOrder, cocos2d::tCCPositionType positionType, cocos2d::CCPoint position) {
            if (config::get<bool>("level.noparticles", false) &&
                config::get<bool>("level.noparticles.nomiscparticles", false))
                return nullptr;

            return GJBaseGameLayer::spawnParticle(plist, zOrder, positionType, position);
        }
    };
}
