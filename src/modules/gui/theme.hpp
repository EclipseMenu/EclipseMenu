#pragma once

#include <memory>

#include "gui.hpp"
#include "color.hpp"

#include "styles/default.hpp"
#include "styles/megahack.hpp"

#include "layouts/window/window.hpp"

#include <Geode/utils/file.hpp>

namespace eclipse::gui::imgui {
    class Theme {
    public:
        explicit Theme(const std::filesystem::path& path) {
            if (!std::filesystem::exists(geode::Mod::get()->getSaveDir() / "themes"))
                std::filesystem::create_directories(geode::Mod::get()->getSaveDir() / "themes");

            if (path.extension() == ".zip")
                loadFromZip(path);
            else
                loadFromFile(path);
        }

        void loadFromZip(const std::filesystem::path& path) {
            auto result = geode::utils::file::Unzip::create(path);

            if (!result) {
                geode::log::error("Failed to extract theme at {}: {}", path, result.error());
                return;
            }

            auto& unzipper = result.value();
            std::vector<std::filesystem::path> entries = unzipper.getEntries();

            uint32_t jsonCount = 0;
            std::string jsonFilename;

            for (const auto& entry : entries) {
                if (entry.extension() == ".json") {
                    jsonFilename = entry.filename().string();
                    jsonCount++;
                }
            }

            if (jsonCount != 1) {
                geode::log::error("Failed to extract theme: not a valid theme (expected 1 json file)");
                return;
            }
            
            auto unzipResult = unzipper.extractTo(jsonFilename, geode::Mod::get()->getSaveDir() / "themes" / jsonFilename);

            if (!unzipResult)
                geode::log::error("Failed to extract theme: {}", unzipResult.error());

            std::ifstream ifs(geode::Mod::get()->getSaveDir() / "themes" / jsonFilename);
            nlohmann::json jf = nlohmann::json::parse(ifs);

            if (jf.contains("options") && jf["options"].contains("font")) {
                std::string fontFile = jf["options"]["font"];
                unzipResult = unzipper.extractTo(fontFile, geode::Mod::get()->getSaveDir() / "fonts" / fontFile);

                if (!unzipResult)
                    geode::log::error("Failed to extract theme font: {}", unzipResult.error());
            }

            loadFromFile(geode::Mod::get()->getSaveDir() / "themes" / jsonFilename);
        }

        /// @brief Load the theme.
        void loadFromFile(const std::filesystem::path& path) {
            std::ifstream ifs(path.c_str());
            nlohmann::json jf = nlohmann::json::parse(ifs);
            if (jf.contains("options")) {
                // we have to do layout early cuz i need m_layout immediately
                if (jf["options"].contains("layout")) {
                    int layNum = jf["options"]["layout"];
                    switch (layNum) {
                        case 0:
                            m_layout = std::static_pointer_cast<Layout>(std::make_shared<WindowLayout>());
                            break;
                        default:
                            m_layout = std::static_pointer_cast<Layout>(std::make_shared<WindowLayout>());
                            break;
                    }
                } else {
                    m_layout = std::static_pointer_cast<Layout>(std::make_shared<WindowLayout>());
                }
                if (jf["options"].contains("style")) m_styleNum = jf["options"]["style"];

                if (jf["options"].contains("font")) {
                    if (!std::filesystem::exists(geode::Mod::get()->getSaveDir() / "fonts"))
                        std::filesystem::create_directories(geode::Mod::get()->getSaveDir() / "fonts");

                    m_font = jf["options"].at("font").get<std::string>();

                    m_fontPath = geode::Mod::get()->getSaveDir() / "fonts" / m_font;
                }
            }
            if (jf.contains("colors")) {
                std::map<int, std::string> m = jf.at("colors").get<std::map<int, std::string>>();
                for (auto[k, v] : m) {
                    m_colors.insert({k, Color::fromString(v)});
                }
            }
            if (jf.contains("floats")) {
                std::map<int, float> m2 = jf.at("floats").get<std::map<int, float>>();
                for (auto[k, v] : m2) {
                    m_floats[k] = v;
                }
            }
        }

        /// @brief Save the theme to a zip file.
        void saveToZip(const std::filesystem::path& path) {
            std::filesystem::path jsonPath = path.parent_path() / (path.stem().string() + ".json");
            saveToFile(jsonPath);

            auto result = geode::utils::file::Zip::create(path);

            if (!result) {
                geode::log::error("Failed to create theme at {}: {}", path, result.error());
                return;
            }

            auto& zipper = result.value();
            static_cast<void>(zipper.addFrom(jsonPath));
            static_cast<void>(zipper.addFrom(m_fontPath));

            std::filesystem::remove(jsonPath);
        }

        /// @brief Save the theme to a file.
        void saveToFile(const std::filesystem::path& path) {
            nlohmann::json j;
            for (auto[k, v] : m_colors) {
                j["colors"][k] = v;
            }
            for (auto[k, v] : m_floats) {
                j["floats"][k] = v;
            }
            j["options"]["font"] = m_font;

            std::ofstream file(path.c_str());
            file << j;
        }

        /// @brief Set up the UI.
        virtual void setup() {
            auto& style = ImGui::GetStyle();
            auto& colors = style.Colors;

            switch (m_styleNum) {
                case 0:
                    m_layout->setStyle(std::make_shared<DefaultStyle>());
                    break;
                case 1:
                    m_layout->setStyle(std::make_shared<MegahackStyle>());
                    break;
                default:
                    m_layout->setStyle(std::make_shared<DefaultStyle>());
                    break;
            }

            for (auto[k, v] : m_colors) {
                colors[static_cast<ImGuiCol_>(k)] = v;
            }
            for (auto[k, v] : m_floats) {
                switch (k) {
                    case 1:
                        style.WindowRounding = v;
                        break;
                    case 2:
                        style.FrameRounding = v;
                        break;
                    case 3:
                        style.PopupRounding = v;
                        break;
                    case 4:
                        style.IndentSpacing = v;
                        break;
                    case 5:
                        style.ScrollbarSize = v;
                        break;
                    case 6:
                        style.ScrollbarRounding = v;
                        break;
                    case 7:
                        style.GrabMinSize = v;
                        break;
                    case 8:
                        style.GrabRounding = v;
                        break;
                    case 9:
                        style.WindowBorderSize = v;
                        break;
                    case 10:
                        style.WindowPadding.x = v;
                        break;
                    case 11:
                        style.WindowPadding.y = v;
                        break;
                    case 12:
                        style.FramePadding.x = v;
                        break;
                    case 13:
                        style.FramePadding.y = v;
                        break;
                    case 14:
                        style.ItemSpacing.x = v;
                        break;
                    case 15:
                        style.ItemSpacing.y = v;
                        break;
                    case 16:
                        style.ItemInnerSpacing.x = v;
                        break;
                    case 17:
                        style.ItemInnerSpacing.y = v;
                        break;
                    case 18:
                        style.WindowTitleAlign.x = v;
                        break;
                    case 19:
                        style.WindowTitleAlign.y = v;
                        break;
                    case 20:
                        style.WindowMinSize.x = v;
                        break;
                    case 21:
                        style.WindowMinSize.y = v;
                        break;
                    case 22:
                        style.DisplayWindowPadding.x = v;
                        break;
                    case 23:
                        style.DisplayWindowPadding.y = v;
                        break;
                }
            }

            if (std::filesystem::exists(m_fontPath)) {
                auto font = ImGui::GetIO().Fonts->AddFontFromFileTTF(m_fontPath.string().c_str(), 20.0f);
                ImGui::GetIO().FontDefault = font;
            }
        }

        /// @brief Get the theme's layout
        [[nodiscard]] std::shared_ptr<Layout> getLayout() { return m_layout; }

    private:
        std::shared_ptr<Layout> m_layout;
        std::string m_font;
        std::filesystem::path m_fontPath;
        std::map<int, Color> m_colors;
        std::map<int, float> m_floats;
        std::map<int, ImVec2> m_vecs;
        int m_styleNum;
    };
}