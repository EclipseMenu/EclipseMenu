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
			config::setIfEmpty("level.legacytrail.max_points", 15); // New config entry

			tab->addToggle("level.legacytrail")
				->handleKeybinds()
				->setDescription("Restore the trail effect from older versions.")
				->addOptions([](std::shared_ptr<gui::MenuTab> options) {
				options->addIntToggle("%i Points", "level.legacytrail.max_points", 10, 20);

					}
				);
		}

			[[nodiscard]] const char* getId() const override { return "Legacy Trail"; }
		};

		REGISTER_HACK(LegacyTrail)

			class $modify(LegacyTrailHook, cocos2d::CCMotionStreak) {
			ADD_HOOKS_DELEGATE("level.legacytrail");


			bool initWithFade(float fade, float minSeg, float stroke, const cocos2d::ccColor3B & color, cocos2d::CCTexture2D * texture) {
				if (config::get<"level.legacytrail", bool>()) {
					this->setPosition(cocos2d::CCPointZero);
					this->setAnchorPoint(cocos2d::CCPointZero);
					this->ignoreAnchorPointForPosition(true);
					m_bStartingPositionInitialized = false;

					m_tPositionR = cocos2d::CCPointZero;
					m_bFastMode = true;
					m_fMinSeg = (minSeg == -1.0f) ? stroke / 5.0f : minSeg;
					m_fMinSeg *= m_fMinSeg;

					m_fStroke = stroke;
					m_fFadeDelta = 1.0f / fade;

					float maxPoints = config::get<"level.legacytrail.max_points", float>();
					m_uMaxPoints = static_cast<unsigned int>(maxPoints);

					m_uNuPoints = 0;

					m_pPointState = (float*)malloc(sizeof(float) * m_uMaxPoints);
					m_pPointVertexes = (cocos2d::CCPoint*)malloc(sizeof(cocos2d::CCPoint) * m_uMaxPoints);

					m_pVertices = (cocos2d::ccVertex2F*)malloc(sizeof(cocos2d::ccVertex2F) * m_uMaxPoints * 2);
					m_pTexCoords = (cocos2d::ccTex2F*)malloc(sizeof(cocos2d::ccTex2F) * m_uMaxPoints * 2);
					m_pColorPointer = (GLubyte*)malloc(sizeof(GLubyte) * m_uMaxPoints * 2 * 4);

					m_tBlendFunc.src = GL_SRC_ALPHA;
					m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

					auto shaderCache = cocos2d::CCShaderCache::sharedShaderCache();
					auto program = shaderCache->programForKey(kCCShader_PositionTextureColor);
					this->setShaderProgram(program);

					this->setTexture(texture);
					this->setColor(color);

					this->scheduleUpdate();

					return true;
				}

				return cocos2d::CCMotionStreak::initWithFade(fade, minSeg, stroke, color, texture);
			}
		};
	}