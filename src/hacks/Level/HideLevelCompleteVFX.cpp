#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCCircleWave.hpp>

namespace eclipse::hacks::Level {

	class HideLevelCompleteVFX : public hack::Hack {
		void init() override {
			auto tab = gui::MenuTab::find("Level");
			tab->addToggle("Hide Level Complete VFX", "level.hidelevelcomplete")
				->setDescription("Hides the explosion and fireworks seen when completing a level. (Does not hide particles.) (Created by RayDeeUx)")
				->handleKeybinds();
		}

		[[nodiscard]] const char* getId() const override { return "Hide Level Complete VFX"; }
	};

	REGISTER_HACK(HideLevelCompleteVFX)

	bool isLevelComplete = false;

	class $modify(EclipseCCCircleWave, CCCircleWave) {
		static CCCircleWave* create(float startRadius, float endRadius, float duration, bool fadeIn, bool easeOut) {
			CCCircleWave* cw = CCCircleWave::create(startRadius, endRadius, duration, fadeIn, easeOut);
			PlayLayer* pl = PlayLayer::get();
			if (!pl) return cw;
			if (pl->m_levelEndAnimationStarted && config::get<bool>("level.hidelevelcomplete", false))
				cw->setVisible(false);
			return cw;
		}
	};
}