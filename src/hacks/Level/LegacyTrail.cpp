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
					options->addIntToggle("Points", "level.legacytrail.max_points", 10, 20);
				});
		}

			[[nodiscard]] const char* getId() const override { return "Legacy Trail"; }
		};

		REGISTER_HACK(LegacyTrail)

		class $modify(LegacyTrailCCMSHook, cocos2d::CCMotionStreak) {
			ADD_HOOKS_DELEGATE("level.legacytrail");

			bool initWithFade(float fade, float minSeg, float stroke, const cocos2d::ccColor3B& color, cocos2d::CCTexture2D* texture) {
				this->setPosition({ .0f, .0f});
				this->setAnchorPoint({ .0f, .0f});
				this->ignoreAnchorPointForPosition(true);
				m_bStartingPositionInitialized = false;

				m_tPositionR = cocos2d::CCPoint{ .0f, .0f };
				m_bFastMode = true;
				m_fMinSeg = (minSeg == -1.f) ? stroke / 5.0f : minSeg;
				m_fMinSeg *= m_fMinSeg;

				m_fStroke = stroke;
				m_fFadeDelta = 1.0f / fade;

				float maxPoints = config::get<"level.legacytrail.max_points", float>();
				m_uMaxPoints = static_cast<unsigned int>(maxPoints);

				m_uNuPoints = 0;

				m_pPointState = std::allocator<float>().allocate(sizeof(float) * m_uMaxPoints);
				m_pPointVertexes = std::allocator<cocos2d::CCPoint>().allocate(sizeof(cocos2d::CCPoint) * m_uMaxPoints);

				m_pVertices = std::allocator<cocos2d::ccVertex2F>().allocate(sizeof(cocos2d::ccVertex2F) * m_uMaxPoints * 2);
				m_pTexCoords = std::allocator<cocos2d::ccTex2F>().allocate(sizeof(cocos2d::ccTex2F) * m_uMaxPoints * 2);
				m_pColorPointer = std::allocator<GLubyte>().allocate(sizeof(GLubyte) * m_uMaxPoints * 2 * 4);

				m_tBlendFunc.src = GL_SRC_ALPHA;
				m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

				auto program = eclipse::utils::get<cocos2d::CCShaderCache>()->programForKey(
					kCCShader_PositionTextureColor
				);
				this->setShaderProgram(program);

				this->setTexture(texture);
				this->setColor(color);

				this->scheduleUpdate();

				return true;
			}
		};
	}