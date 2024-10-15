#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class SmartStartPos : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Smart StartPos", "level.smartstartpos")
                ->handleKeybinds()
                ->setDescription("Makes the start positions automatically pick the correct gamemode and speed. (This only works in levels without directional changes.)");
        }

        [[nodiscard]] const char* getId() const override { return "Smart StartPos"; }
    };

    REGISTER_HACK(SmartStartPos)

    class $modify(SmartStartPosPLHook, PlayLayer) {
        struct Fields {
            std::vector<StartPosObject*> m_startPositions;
            std::vector<GameObject*> m_dualPortals, m_gamemodePortals, m_miniPortals, m_speedChanges, m_mirrorPortals;
        };

        GameObject* getClosestObject(std::vector<GameObject*>& vec, StartPosObject* startPos) {
            GameObject* closest = nullptr;

            std::ranges::sort(vec, [] (GameObject* a, GameObject* b) {
                return a->getPositionX() < b->getPositionX();
            });

            for (auto obj : vec) {
                if (obj->getPositionX() - 10 > startPos->getPositionX())
                    break;
                if (obj->getPositionX() - 10 < startPos->getPositionX())
                    closest = obj;
            }

            return closest;
        }

        void setupStartPos(StartPosObject* startPos) {
            LevelSettingsObject* startPosSettings = startPos->m_startSettings;
            LevelSettingsObject* levelSettings = PlayLayer::get()->m_levelSettings;

            startPosSettings->m_startDual = levelSettings->m_startDual;
            startPosSettings->m_startMode = levelSettings->m_startMode;
            startPosSettings->m_startMini = levelSettings->m_startMini;
            startPosSettings->m_startSpeed = levelSettings->m_startSpeed;

            GameObject* obj = getClosestObject(m_fields->m_dualPortals, startPos);
            if (obj)
                startPosSettings->m_startDual = obj->m_objectID == 286;

            obj = getClosestObject(m_fields->m_gamemodePortals, startPos);

            if (obj) {
                switch (obj->m_objectID) {
                    case 12:
                        startPosSettings->m_startMode = 0;
                        break;
                    case 13:
                        startPosSettings->m_startMode = 1;
                        break;
                    case 47:
                        startPosSettings->m_startMode = 2;
                        break;
                    case 111:
                        startPosSettings->m_startMode = 3;
                        break;
                    case 660:
                        startPosSettings->m_startMode = 4;
                        break;
                    case 745:
                        startPosSettings->m_startMode = 5;
                        break;
                    case 1331:
                        startPosSettings->m_startMode = 6;
                        break;
                    case 1933:
                        startPosSettings->m_startMode = 7;
                        break;
                }
            }

            auto fields = m_fields.self();

            obj = getClosestObject(fields->m_miniPortals, startPos);

            if (obj)
                startPosSettings->m_startMini = obj->m_objectID == 101;

            obj = getClosestObject(fields->m_speedChanges, startPos);
            if (obj) {
                switch (obj->m_objectID) {
                    case 200:
                        startPosSettings->m_startSpeed = Speed::Slow;
                        break;
                    case 201:
                        startPosSettings->m_startSpeed = Speed::Normal;
                        break;
                    case 202:
                        startPosSettings->m_startSpeed = Speed::Fast;
                        break;
                    case 203:
                        startPosSettings->m_startSpeed = Speed::Faster;
                        break;
                    case 1334:
                        startPosSettings->m_startSpeed = Speed::Fastest;
                        break;
                }
            }
        }

        bool init(GJGameLevel* level, bool unk1, bool unk2) {
            auto fields = m_fields.self();
            fields->m_dualPortals.clear();
            fields->m_gamemodePortals.clear();
            fields->m_miniPortals.clear();
            fields->m_miniPortals.clear();
            fields->m_speedChanges.clear();
            fields->m_mirrorPortals.clear();
            fields->m_startPositions.clear();

            return PlayLayer::init(level, unk1, unk2);
        }

        void resetLevel() {
            if (config::get<bool>("level.smartstartpos", false)) {
                for (StartPosObject* obj : m_fields->m_startPositions)
                    setupStartPos(obj);
            }

            PlayLayer::resetLevel();
        }

        void addObject(GameObject* obj) {
            PlayLayer::addObject(obj);

            switch (obj->m_objectID)
            {
                case 31:
                    m_fields->m_startPositions.push_back(static_cast<StartPosObject *>(obj));
                    break;
                case 12:
                case 13:
                case 47:
                case 111:
                case 660:
                case 745:
                case 1331:
                case 1933:
                    m_fields->m_gamemodePortals.push_back(obj);
                    break;
                case 45:
                case 46:
                    m_fields->m_mirrorPortals.push_back(obj);
                    break;
                case 99:
                case 101:
                    m_fields->m_miniPortals.push_back(obj);
                    break;
                case 286:
                case 287:
                    m_fields->m_dualPortals.push_back(obj);
                    break;
                case 200:
                case 201:
                case 202:
                case 203:
                case 1334:
                    m_fields->m_speedChanges.push_back(obj);
                    break;
            }
        }

    };
}