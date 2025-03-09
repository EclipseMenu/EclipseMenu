#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/FMODAudioEngine.hpp>


namespace eclipse::hacks::Level {
	class $hack(LegacyPulse) {
		void init() override {
			auto tab = gui::MenuTab::find("tab.level");

			config::setIfEmpty("level.legacypulse", false);
			config::setIfEmpty("level.legacypulse.force_fps", false);
			config::setIfEmpty("level.legacypulse.fps", 60.f);

			tab->addToggle("level.legacypulse")
				->handleKeybinds()
				->setDescription("Restore the pulse effect from older versions.")
				->addOptions([](std::shared_ptr<gui::MenuTab> options) {
					options->addToggle("level.legacypulse.force_fps")
						->setDescription("Enable fixed FPS for legacy pulse simulation")
						->addOptions([](std::shared_ptr<gui::MenuTab> fpsOptions) {
							fpsOptions->addInputFloat("level.legacypulse.fps", 1.f, FLT_MAX, "%.0f FPS");
						});
				});
		}

		[[nodiscard]] const char* getId() const override { return "Legacy Pulse"; }
	};

	void updarePulseOldLogic(FMODAudioEngine* self) {
		self->m_system->update();

		if (self->m_metering != false) {
			FMOD_DSP_METERING_INFO meteringInfo = {};
			self->m_mainDSP->getMeteringInfo(0x0, &meteringInfo);

			if (meteringInfo.numchannels == 2) {
				float baseVolume = self->m_musicVolume;
				float leftChannelLevel = meteringInfo.peaklevel[0];
				float rightChannelLevel = meteringInfo.peaklevel[1];

				if (baseVolume > 0.0f) {
					leftChannelLevel /= baseVolume;
					rightChannelLevel /= baseVolume;
				}

				float averageLevel = (leftChannelLevel + rightChannelLevel) * 0.5f + 0.1f;
				self->m_pulse1 = averageLevel;

				int* pulseCounter = &self->m_pulseCounter;
				float* previousAverageLevel = &self->m_pulse3;
				float* previousPulseLevel = &self->m_pulse2;

				if ((*pulseCounter < 3) ||
					(averageLevel < *previousPulseLevel * 1.1f) ||
					(averageLevel < *previousAverageLevel * 0.95f && *previousAverageLevel * 0.2f < *previousPulseLevel)) {

					self->m_pulse1 = *previousPulseLevel * 0.93f;
				}
				else {
					*previousAverageLevel = averageLevel;
					*pulseCounter = 0;
					self->m_pulse1 = averageLevel * 1.1f;
				}

				if (self->m_pulse1 <= 0.1f) {
					*previousAverageLevel = 0.0f;
				}

				(*pulseCounter)++;
				*previousPulseLevel = self->m_pulse1;
			}
		}
	}

	REGISTER_HACK(LegacyPulse)

	class $modify(LegacyPulseFMODHook, FMODAudioEngine) {
		ADD_HOOKS_DELEGATE("level.legacypulse");

		void update(float deltaTime) {
			static float accumulatedTime = 0.0f;

			FMODAudioEngine::update(deltaTime);

			bool forceFPS = config::get<"level.legacypulse.force_fps", bool>();

			if (forceFPS) {
				float fixedTimestep = 1.0f / config::get<float>("level.legacypulse.fps", 60.f);

				accumulatedTime += deltaTime;
				while (accumulatedTime >= fixedTimestep) {
					updarePulseOldLogic(this);
					accumulatedTime -= fixedTimestep;
				}
			}
			else {
				updarePulseOldLogic(this);
				accumulatedTime = 0.0f;
			}
		}
	};
}