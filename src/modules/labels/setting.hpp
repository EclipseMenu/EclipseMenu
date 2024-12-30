#pragma once
#include <hacks/Labels/LabelContainer.hpp>
#include <modules/gui/color.hpp>
#include <nlohmann/json.hpp>
#include "events.hpp"

namespace eclipse::labels {
    using LabelsContainer = hacks::Labels::LabelsContainer;

    inline static std::array alignmentNames = {
        "Top Left", "Top Center", "Top Right",
        "Center Left", "Center", "Center Right",
        "Bottom Left", "Bottom Center", "Bottom Right"
    };

    constexpr std::array fontFiles = {
        "bigFont.fnt", "chatFont.fnt", "goldFont.fnt", "gjFont01.fnt",
        "gjFont02.fnt", "gjFont03.fnt", "gjFont04.fnt", "gjFont05.fnt",
        "gjFont06.fnt", "gjFont07.fnt", "gjFont08.fnt", "gjFont09.fnt",
        "gjFont10.fnt", "gjFont11.fnt", "gjFont12.fnt", "gjFont13.fnt",
        "gjFont14.fnt", "gjFont15.fnt", "gjFont16.fnt", "gjFont17.fnt",
        "gjFont18.fnt", "gjFont19.fnt", "gjFont20.fnt", "gjFont21.fnt",
        "gjFont22.fnt", "gjFont23.fnt", "gjFont24.fnt", "gjFont25.fnt",
        "gjFont26.fnt", "gjFont27.fnt", "gjFont28.fnt", "gjFont29.fnt",
        "gjFont30.fnt", "gjFont31.fnt", "gjFont32.fnt", "gjFont33.fnt",
        "gjFont34.fnt", "gjFont35.fnt", "gjFont36.fnt", "gjFont37.fnt",
        "gjFont38.fnt", "gjFont39.fnt", "gjFont40.fnt", "gjFont41.fnt",
        "gjFont42.fnt", "gjFont43.fnt", "gjFont44.fnt", "gjFont45.fnt",
        "gjFont46.fnt", "gjFont47.fnt", "gjFont48.fnt", "gjFont49.fnt",
        "gjFont50.fnt", "gjFont51.fnt", "gjFont52.fnt", "gjFont53.fnt",
        "gjFont54.fnt", "gjFont55.fnt", "gjFont56.fnt", "gjFont57.fnt",
        "gjFont58.fnt", "gjFont59.fnt",
    };

    constexpr std::array fontNames = {
        "Pusab", "Aller", "Gold Pusab", "TRS Million",
        "SF Distant Galaxy", "Relish Gargler", "Early GameBoy", "Good Dog",
        "Dancing Script", "Stencilla", "Xirod", "Minercraftory",
        "Super Mario 256", "Ketchum", "Gargle", "Amatic",
        "Cartwheel", "Mothproof Script", "Lemon Milk Bold", "Lemon Milk",
        "Minecraft", "Optimus Princeps", "Autolova", "Karate",
        "a Annyeong Haseyo", "Ausweis Hollow", "Gypsy Curse", "Magic School Two",
        "Old English Five", "Yeah Papa", "Ninja Naruto", "Metal Lord",
        "Drip Drop", "Electroharmonix", "Aladin", "Creepster",
        "Call Of Ops Duty", "BlocParty", "Astron Boy", "Osaka-Sans Serif",
        "Some Time Later", "Fatboy Slim BLTC BRK", "Moria Citadel", "Rise of Kingdom",
        "Fantaisie Artistique", "Edge of the Galaxy", "Wash Your Hand", "Bitwise",
        "Foul Fiend", "Nandaka Western", "Evil Empire", "Comical Cartoon",
        "Carton Six", "aAssassinNinja", "Public Pixel", "New Walt Disney UI",
        "Random 5", "Crafting Lesson", "Game Of Squids", "Monster Game",
        "Lo-Sumires", "Gewtymol",
    };

    inline int32_t getFontIndex(const std::string& font) {
        auto it = std::ranges::find(fontFiles, font);
        size_t index = it == fontFiles.end() ? 0 : std::distance(fontFiles.begin(), it);
        return static_cast<int32_t>(index);
    }

    /// @brief Settings for a label to store in the config.
    struct LabelSettings {
        static size_t instanceCount; // used to generate unique IDs

        ~LabelSettings() {
            // release all current related events
            EventManager::get().removeEvents(this);
        }

        std::string name;
        std::string text;
        bool visible = true;
        bool absolutePosition = false;
        float scale = 0.35f;
        gui::Color color = gui::Color(1.f, 1.f, 1.f, 0.3f);
        std::string font = "bigFont.fnt";
        LabelsContainer::Alignment alignment = LabelsContainer::Alignment::TopLeft;
        cocos2d::CCPoint offset = {0, 0};
        std::vector<LabelEvent> events;
        size_t id = instanceCount++;

        Event::EventState processEvents() const;
        bool hasEvents() const { return !events.empty(); }

        void promptSave() const;
    };

    void from_json(const nlohmann::json& json, LabelSettings& settings);
    void to_json(nlohmann::json& json, const LabelSettings& settings);
    void from_json(const nlohmann::json& json, LabelEvent& event);
    void to_json(nlohmann::json& json, const LabelEvent& event);
}
