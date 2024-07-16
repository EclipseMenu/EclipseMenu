#pragma once

#include "gui.hpp"
#include "color.hpp"

#include "styles/default.hpp"
#include "styles/megahack.hpp"

#include "layouts/window/window.hpp"

namespace eclipse::gui::imgui {
    class Theme {
    public:
        explicit Theme(const std::filesystem::path& path) {
            loadFromFile(path);
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
                            m_layout = new WindowLayout();
                            break;
                        default:
                            m_layout = new WindowLayout();
                            break;
                    }
                } else {
                    m_layout = new WindowLayout();
                }
                if (jf["options"].contains("style")) m_styleNum = jf["options"]["style"];
            }
            if (jf.contains("colors")) {
                std::map<int, Color> m = jf.at("colors").get<std::map<int, Color>>();
                for (auto[k, v] : m) {
                    m_colors[k] = v;
                }
            }
            if (jf.contains("floats")) {
                std::map<int, float> m2 = jf.at("floats").get<std::map<int, float>>();
                for (auto[k, v] : m2) {
                    m_floats[k] = v;
                }
            }
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
            std::ofstream file(path.c_str());
            file << j;
        }

        /// @brief Set up the UI.
        virtual void setup() {
            auto &style = ImGui::GetStyle();
            auto &colors = style.Colors;

            switch (m_styleNum) {
                case 0:
                    m_layout->setStyle(new DefaultStyle());
                    break;
                case 1:
                    m_layout->setStyle(new MegahackStyle());
                    break;
                default:
                    m_layout->setStyle(new DefaultStyle());
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
                }
            }
        }

        /// @brief Get the theme's layout
        [[nodiscard]] Layout* getLayout() { return m_layout; }

    private:
        Layout* m_layout;
        std::map<int, Color> m_colors;
        std::map<int, float> m_floats;
        std::map<int, ImVec2> m_vecs;
        int m_styleNum;
    };
}