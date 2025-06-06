#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CCMotionStreak.hpp>

namespace eclipse::hacks::Level {
	class $hack(LegacyTrail) {
		void init() override {
			auto tab = gui::MenuTab::find("tab.level");

			config::setIfEmpty("level.legacytrail", false);
			config::setIfEmpty("level.legacytrail.force_max_points", false);
			config::setIfEmpty("level.legacytrail.max_points", 18); // New config entry

			tab->addToggle("level.legacytrail")
				->handleKeybinds()
				->setDescription("Restore the trail effect from older versions.")
				->addOptions([](std::shared_ptr<gui::MenuTab> options) {
					options->addIntToggle("level.legacytrail.max_points", 2, 36);
				});
		}

			[[nodiscard]] const char* getId() const override { return "Legacy Trail"; }
		};

		REGISTER_HACK(LegacyTrail)

		class $modify(LegacyTrailCCMSHook, cocos2d::CCMotionStreak) {
			ADD_HOOKS_DELEGATE("level.legacytrail");

			bool initWithFade(float fade, float minSeg, float stroke, const cocos2d::ccColor3B& color, cocos2d::CCTexture2D* texture) {
				if (!cocos2d::CCMotionStreak::initWithFade(fade, minSeg, stroke, color, texture))
					return false;

				float maxPoints = config::get<"level.legacytrail.max_points", float>();
				m_uMaxPoints = static_cast<unsigned int>(maxPoints);

				auto program = eclipse::utils::get<cocos2d::CCShaderCache>()->programForKey(
					kCCShader_PositionTextureColor
				);
				this->setShaderProgram(program);

				return true;
			}
		};
	}
