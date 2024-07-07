#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

namespace eclipse::hacks::Global {

    void setSpeed(FMOD::Channel* channel) {
        float speed = 1.f;

        if(config::get<bool>("global.audiospeed.toggle", false))
            speed = config::get<float>("global.audiospeed", 1.f);

        FMOD::Sound* sound;
        channel->getCurrentSound(&sound);
        float freq;
        sound->getDefaults(&freq, nullptr);
        channel->setFrequency(freq * speed);
    }
    void speedhackSync() {
        float speedhack = config::get<float>("global.speedhack", 1.f);
        config::set<float>("global.audiospeed", speedhack);      
    }
    void updateChannels() {
        FMOD::Channel* audioChannel;
        FMOD::System *system = FMODAudioEngine::sharedEngine()->m_system;
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

            auto floatToggle = tab->addFloatToggle("Audio Speed", "global.audiospeed", 0.0001f, 1000.f, "%.4f");
            tab->addButton("Sync w/ Speedhack")->callback(speedhackSync);
            floatToggle->toggleCallback(updateChannels);
        }

        void lateInit() override {}

        void update() override {
            if(!config::get<bool>("global.audiospeed.toggle", false))
                return;

            updateChannels();
        }

        [[nodiscard]] const char* getId() const override { return "Audio Speed"; }
        [[nodiscard]] int32_t getPriority() const override { return -9; }
    };

    REGISTER_HACK(AudioSpeed)

}
