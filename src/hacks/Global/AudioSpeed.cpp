#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

namespace eclipse::hacks::Global {

    void setSpeed(FMOD::Channel* channel) {
        float speed = 1.f;

        if (config::get<bool>("global.audiospeed.sync", false)) {
            bool speedhack = config::get<bool>("global.speedhack.toggle", false);
            speed = speedhack ? config::get<float>("global.speedhack", 1.f) : 1.f;
        }

        if (config::get<bool>("global.audiospeed.toggle", false))
            speed = config::get<float>("global.audiospeed", 1.f);

        FMOD::Sound* sound;
        channel->getCurrentSound(&sound);
        float freq;
        sound->getDefaults(&freq, nullptr);
        channel->setFrequency(freq * speed);
    }

    void updateChannels() {
        FMOD::Channel* audioChannel;
        FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;

        for (auto i = 0; i < 4; i++) {
            system->getChannel(126 + i, &audioChannel);
            if (audioChannel)
                setSpeed(audioChannel);
        }
    }

    class AudioSpeed : public hack::Hack {

        void init() override {
            auto tab = gui::MenuTab::find("Global");

            config::setIfEmpty("global.audiospeed.toggle", false);
            config::setIfEmpty("global.audiospeed", 1.f);

            tab->addFloatToggle("Audio Speed", "global.audiospeed", 0.0001f, 1000.f, "%.4f")
                ->handleKeybinds()
                ->toggleCallback(updateChannels);

            tab->addToggle("Sync with Speedhack", "global.audiospeed.sync")
                ->handleKeybinds()
                ->callback([](bool){ updateChannels(); })
                ->setDescription("Sync audio speed with speedhack, if it's enabled.");
        }

        void update() override {
            bool speedhack = config::get<bool>("global.speedhack.toggle", false);
            bool toggledSpeedhack = m_lastSpeedhackState != speedhack;
            m_lastSpeedhackState = speedhack;

            bool audiospeed = config::get<bool>("global.audiospeed.toggle", false);
            bool sync = config::get<bool>("global.audiospeed.sync", false);

            // With sync: update if speedhack is on or has been toggled
            // Without sync: update if audiospeed is on
            if (((speedhack || toggledSpeedhack) && sync) || audiospeed)
                updateChannels();
        }

        [[nodiscard]] const char* getId() const override { return "Audio Speed"; }
        [[nodiscard]] int32_t getPriority() const override { return -8; }

        bool m_lastSpeedhackState = false;
    };

    REGISTER_HACK(AudioSpeed)

}
