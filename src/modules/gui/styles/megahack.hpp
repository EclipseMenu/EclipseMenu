#pragma once

#include <modules/gui/gui.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <modules/config/config.hpp>
#include <imgui-cocos.hpp>

namespace eclipse::gui::imgui {
    class MegahackStyle : public Style {
    public:
        /// @brief Special tooltip function that makes sure the tooltip fits on the screen.
        static void handleTooltip(const std::string& text) {
            if (text.empty()) return;
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImVec2 pos = ImGui::GetMousePos();

                auto size = ImGui::CalcTextSize(text.c_str());
                size.x /= config::getTemp<float>("UIScale", 1.0f);
                auto screenSize = ImGui::GetIO().DisplaySize;

                const float padding = 10.0f;
                pos.x += padding;
                pos.y += padding;

                if (pos.x + size.x > screenSize.x)
                    pos.x = screenSize.x - size.x - padding;
                if (pos.y + size.y > screenSize.y)
                    pos.y = screenSize.y - size.y - padding;

                ImGui::SetNextWindowPos(pos);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
                ImGui::SetTooltip("%s", text.c_str());
                ImGui::PopStyleColor();
            }
        }

        void visit(LabelComponent* label) override {
            auto availWidth = ImGui::GetContentRegionAvail().x;
            float textWidth = ImGui::CalcTextSize(label->getTitle().c_str(), nullptr, true, availWidth).x;

            float posX = (availWidth - textWidth) * 0.5f;
            if (posX > 0.0f)
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + posX);

            ImGui::TextWrapped("%s", label->getTitle().c_str());
        }   

        void visit(ToggleComponent* checkbox) override {
            auto& style = ImGui::GetStyle();
            auto& colors = style.Colors;
            bool value = checkbox->getValue();

            auto handleKeybindMenu = [checkbox] {
                // Open context menu on either Right click or Shift+Click
                if (ImGui::IsItemClicked(1) || (ImGui::IsItemClicked(0) && ImGui::GetIO().KeyShift)) {
                    ImGui::OpenPopup(fmt::format("##context-menu-{}", checkbox->getId()).c_str());
                }

                if (ImGui::BeginPopup(fmt::format("##context-menu-{}", checkbox->getId()).c_str())) {
                    auto keybinds = keybinds::Manager::get();
                    auto keybind = keybinds->getKeybind(checkbox->getId());

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

                    if (keybind.has_value()) {
                        auto& keybindRef = keybind->get();

                        if (!keybindRef.isInitialized() && ImGui::MenuItem("Add keybind"))
                            keybinds->setKeybindState(checkbox->getId(), true);
                        else if (keybindRef.isInitialized() && ImGui::MenuItem("Remove keybind"))
                            keybinds->setKeybindState(checkbox->getId(), false);
                    }
                    ImGui::PopStyleColor();

                    ImGui::EndPopup();
                }
            };

            if (auto options = checkbox->getOptions().lock()) {
                ImGui::PushItemWidth(-1);

                auto textColor = value ? colors[ImGuiCol_Text] : colors[ImGuiCol_TextDisabled];
                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)textColor);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

                auto availWidth = ImGui::GetContentRegionAvail().x;
                auto buttonSize = ImVec2(availWidth * 0.9f, 0);
                auto arrowSize = ImVec2(availWidth * 0.1f, 0);

                if (ImGui::Button(checkbox->getTitle().c_str(), buttonSize)) {
                    checkbox->setValue(!value);
                    checkbox->triggerCallback(!value);
                }
                handleTooltip(checkbox->getDescription());

                if (checkbox->hasKeybind())
                    handleKeybindMenu();

                ImGui::SameLine(0, 0);

                ImGui::PopStyleVar(2);
                bool openPopup = ImGui::Button(fmt::format("##open_{}", checkbox->getTitle()).c_str(), arrowSize);
                ImGui::PopItemWidth();
                ImGui::PopStyleColor(4);

                auto scale = 1.f;
                auto top = ImGui::GetItemRectMin().y + (4.5 * scale);
                auto bottom = ImGui::GetItemRectMax().y - (4.5 * scale);
                auto right = ImGui::GetItemRectMax().x - (4.5 * scale);
                auto side = bottom - top;
                auto left = right - side;
                auto triangleColor = colors[ImGuiCol_TextDisabled];
                triangleColor.w *= ImGui::GetStyle().Alpha;
                ImGui::GetWindowDrawList()->AddTriangleFilled(
                    ImVec2(right, top),
                    ImVec2(left, bottom),
                    ImVec2(right, bottom),
                    ImGui::ColorConvertFloat4ToU32(triangleColor)
                );

                std::string popupName = fmt::format("##{}", checkbox->getTitle());
                if (openPopup)
                    ImGui::OpenPopup(popupName.c_str());

                ImGui::SetNextWindowSizeConstraints(ImVec2(240, 0), ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopup(popupName.c_str())) {
                    for (auto cmp : options->getComponents())
                        Style::visit(cmp.get());

                    ImGui::EndPopup();
                }
            } else {
                ImGui::PushItemWidth(-1.0f);

                auto textColor = value ? colors[ImGuiCol_Text] : colors[ImGuiCol_TextDisabled];

                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

                if (ImGui::Button(checkbox->getTitle().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                    checkbox->setValue(!value);
                    checkbox->triggerCallback(!value);
                }
                handleTooltip(checkbox->getDescription());

                if (checkbox->hasKeybind())
                    handleKeybindMenu();

                ImGui::PopStyleColor(4);
                ImGui::PopStyleVar();

                auto scale = 1.f;

                textColor.w *= ImGui::GetStyle().Alpha;
                ImGui::GetWindowDrawList()->AddRectFilled(
                        ImVec2(ImGui::GetItemRectMax().x - 5 * scale, ImGui::GetItemRectMin().y + 1 * scale),
                        ImVec2(ImGui::GetItemRectMax().x - 2 * scale, ImGui::GetItemRectMax().y - 1 * scale),
                        ImGui::ColorConvertFloat4ToU32(textColor));

                ImGui::PopItemWidth();
            }
        }   

        void visit(SliderComponent* slider) override {
            auto value = config::get<float>(slider->getId(), 0.f);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);
            if (ImGui::SliderFloat(slider->getTitle().c_str(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
                config::set(slider->getId(), value);
                slider->triggerCallback(value);
            }
            handleTooltip(slider->getDescription());
            ImGui::PopItemWidth();
        }   

        void visit(InputFloatComponent* inputFloat) override {
            auto value = config::get<float>(inputFloat->getId(), 0.f);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
            if (ImGui::InputFloat(inputFloat->getTitle().c_str(), &value, 0, 0, inputFloat->getFormat().c_str())) {
                value = std::clamp(value, inputFloat->getMin(), inputFloat->getMax());
                config::set(inputFloat->getId(), value);
                inputFloat->triggerCallback(value);
            }
            handleTooltip(inputFloat->getDescription());
            ImGui::PopItemWidth();
        }   

        void visit(InputIntComponent* inputInt) override {
            auto value = config::get<int>(inputInt->getId(), 0);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
            if (ImGui::InputInt(inputInt->getTitle().c_str(), &value, 0, 0)) {
                value = std::clamp(value, inputInt->getMin(), inputInt->getMax());
                config::set(inputInt->getId(), value);
                inputInt->triggerCallback(value);
            }
            handleTooltip(inputInt->getDescription());
            ImGui::PopItemWidth();
        }   

        void visit(FloatToggleComponent* floatToggle) override {
            auto value = config::get<float>(floatToggle->getId(), 0.0f);
            bool toggle = config::get<bool>(floatToggle->getId() + ".toggle", false);

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
            if (ImGui::InputFloat(fmt::format("##{}", floatToggle->getTitle()).c_str(), &value, 0, 0, floatToggle->getFormat().c_str())) {
                value = std::clamp(value, floatToggle->getMin(), floatToggle->getMax());
                config::set(floatToggle->getId(), value);
                floatToggle->triggerCallback(value);
            }
            ImGui::PopItemWidth();

            ImGui::SameLine(0, 1);

            ImGui::PushItemWidth(-1.0f);

            auto& style = ImGui::GetStyle();
            auto& colors = style.Colors;

            auto textColor = toggle ? colors[ImGuiCol_Text] : colors[ImGuiCol_TextDisabled];

            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4) textColor);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

            if (ImGui::Button(floatToggle->getTitle().c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
                toggle = !toggle;
                config::set(fmt::format("{}.toggle", floatToggle->getId()), toggle);
                floatToggle->triggerCallback();
            }
            handleTooltip(floatToggle->getDescription());

            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();

            if (floatToggle->hasKeybind()) {
                // Open context menu on either Right click or Shift+Click
                auto id = fmt::format("##context-menu-{}", floatToggle->getId());
                if (ImGui::IsItemClicked(1) || (ImGui::IsItemClicked(0) && ImGui::GetIO().KeyShift)) {
                    ImGui::OpenPopup(id.c_str());
                }

                if (ImGui::BeginPopup(id.c_str())) {
                    auto keybinds = keybinds::Manager::get();
                    auto keybind = keybinds->getKeybind(floatToggle->getId());

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

                    if (keybind.has_value()) {
                        auto& keybindRef = keybind->get();

                        if (!keybindRef.isInitialized() && ImGui::MenuItem("Add keybind"))
                            keybinds->setKeybindState(floatToggle->getId(), true);
                        else if (keybindRef.isInitialized() && ImGui::MenuItem("Remove keybind"))
                            keybinds->setKeybindState(floatToggle->getId(), false);
                    }
                    ImGui::PopStyleColor();

                    ImGui::EndPopup();
                }
            }

            auto scale = 1.f;

            textColor.w *= ImGui::GetStyle().Alpha;
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(ImGui::GetItemRectMax().x - 5 * scale, ImGui::GetItemRectMin().y + 1 * scale),
                ImVec2(ImGui::GetItemRectMax().x - 2 * scale, ImGui::GetItemRectMax().y - 1 * scale),
                ImGui::ColorConvertFloat4ToU32(textColor)
            );

            ImGui::PopItemWidth();
        }   

        void visit(RadioButtonComponent* radioButton) override {
            int value = config::get<int>(radioButton->getId(), radioButton->getValue());
            ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1, 1, 1, 1));
            if (ImGui::RadioButton(radioButton->getTitle().c_str(), &value, radioButton->getValue())) {
                config::set(radioButton->getId(), value);
                radioButton->triggerCallback(value);
            }
            handleTooltip(radioButton->getDescription());
            ImGui::PopStyleColor();

            if (radioButton->hasKeybind()) {
                // Open context menu on either Right click or Shift+Click
                auto id = fmt::format("##context-menu-{}-{}", radioButton->getId(), radioButton->getValue());
                if (ImGui::IsItemClicked(1) || (ImGui::IsItemClicked(0) && ImGui::GetIO().KeyShift)) {
                    ImGui::OpenPopup(id.c_str());
                }

                if (ImGui::BeginPopup(id.c_str())) {
                    auto keybinds = keybinds::Manager::get();
                    auto specialId = fmt::format("{}-{}", radioButton->getId(), radioButton->getValue());
                    auto keybind = keybinds->getKeybind(specialId);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

                    if (keybind.has_value()) {
                        auto& keybindRef = keybind->get();

                        if (!keybindRef.isInitialized() && ImGui::MenuItem("Add keybind")) {
                            keybinds->setKeybindState(specialId, true);
                        } else if (keybindRef.isInitialized() && ImGui::MenuItem("Remove keybind")) {
                            keybinds->setKeybindState(specialId, false);
                        }
                    }
                    ImGui::PopStyleColor();

                    ImGui::EndPopup();
                }
            }
        }   

        void visit(ComboComponent* combo) override {
            auto& items = combo->getItems();
            int value = config::get<int>(combo->getId(), combo->getValue());
            if (ImGui::BeginCombo(combo->getTitle().c_str(), items[value].c_str())) {
                for (int n = 0; n < items.size(); n++) {
                    const bool is_selected = (value == n);
                    if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                        config::set(combo->getId(), n);
                        combo->triggerCallback(n);
                    }

                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }   

        void visit(InputTextComponent* inputText) override {
            auto value = config::get<std::string>(inputText->getId(), "");
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
            if (ImGui::InputText(inputText->getTitle().c_str(), &value)) {
                config::set(inputText->getId(), value);
                inputText->triggerCallback(value);
            }
            handleTooltip(inputText->getDescription());
            ImGui::PopItemWidth();
        }   

        void visit(ButtonComponent* button) override {
            auto& style = ImGui::GetStyle();
            auto& colors = style.Colors;
            ImGui::PushItemWidth(-1);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            auto availWidth = ImGui::GetContentRegionAvail().x;

            if (ImGui::Button(button->getTitle().c_str(), ImVec2(availWidth, 0))) {
                button->triggerCallback();
            }
            handleTooltip(button->getDescription());

            ImGui::PopStyleColor(3);

            if (button->hasKeybind()) {
                // Open context menu on either Right click or Shift+Click
                auto id = fmt::format("##context-menu-btn-{}", button->getId());
                if (ImGui::IsItemClicked(1) || (ImGui::IsItemClicked(0) && ImGui::GetIO().KeyShift)) {
                    ImGui::OpenPopup(id.c_str());
                }

                if (ImGui::BeginPopup(id.c_str())) {
                    auto keybinds = keybinds::Manager::get();
                    auto specialId = fmt::format("button.{}", button->getId());
                    auto keybind = keybinds->getKeybind(specialId);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

                    if (keybind.has_value()) {
                        auto& keybindRef = keybind->get();

                        if (!keybindRef.isInitialized() && ImGui::MenuItem("Add keybind")) {
                            keybinds->setKeybindState(specialId, true);
                        } else if (keybindRef.isInitialized() && ImGui::MenuItem("Remove keybind")) {
                            keybinds->setKeybindState(specialId, false);
                        }
                    }
                    ImGui::PopStyleColor();

                    ImGui::EndPopup();
                }
            }

            // Draw two lines
            bool isMouseOver = ImGui::IsItemHovered();
            bool isItemActive = ImGui::IsItemActive();

            Color color;
            if (isMouseOver) {
                if (isItemActive) {
                    color = colors[ImGuiCol_ButtonActive];
                } else {
                    color = colors[ImGuiCol_ButtonHovered];
                }
            } else {
                color = colors[ImGuiCol_Button];
            }

            color.a *= ImGui::GetStyle().Alpha;
            auto scale = 1.f; // TODO: change with scaling

            ImGui::GetWindowDrawList()->AddLine(
                    ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMin().y + 1),
                    ImVec2(ImGui::GetItemRectMin().x + 1, ImGui::GetItemRectMax().y - 3),
                    color, 2.5f * scale);

            ImGui::GetWindowDrawList()->AddLine(
                    ImVec2(ImGui::GetItemRectMax().x - 2, ImGui::GetItemRectMin().y + 1),
                    ImVec2(ImGui::GetItemRectMax().x - 2, ImGui::GetItemRectMax().y - 3),
                    color, 2.5f * scale);

            ImGui::PopItemWidth();
        }   

        void visit(ColorComponent* color) override {
            auto value = config::get<gui::Color>(color->getId(), {1, 1, 1, 1});
            if (ImGui::ColorEdit3(color->getTitle().c_str(), value.data(), ImGuiColorEditFlags_NoInputs)) {
                config::set<gui::Color>(color->getId(), value);
                color->triggerCallback(value);
            }
            handleTooltip(color->getDescription());
        }   

        void visit(KeybindComponent* keybind) override {
            auto& title = keybind->getTitle();
            auto canDelete = keybind->canDelete();

            ImGui::PushID(title.c_str());
            ImGui::PushItemWidth(-1);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

            // truncate title if it's too long
            auto availWidth = ImGui::GetContentRegionAvail().x;
            auto labelMaxWidth = availWidth * (canDelete ? 0.5f : 0.6f);
            auto labelSize = ImGui::CalcTextSize(title.c_str());

            if (labelSize.x > labelMaxWidth) {
                auto labelEnd = 0;
                while (labelEnd != title.size()) {
                    auto labelStr = title.substr(0, labelEnd) + "...";
                    auto newSize = ImGui::CalcTextSize(labelStr.c_str());
                    if (newSize.x > labelMaxWidth - 20)
                        break;
                    labelEnd++;
                }
                auto truncatedLabel = title.substr(0, labelEnd) + "...";
                ImGui::Button(truncatedLabel.c_str(), ImVec2(labelMaxWidth, 0));
                handleTooltip(title);
            } else {
                ImGui::Button(title.c_str(), ImVec2(labelMaxWidth, 0));
            }

            ImGui::SameLine(0, 2);

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            auto key = config::get<keybinds::Keys>(keybind->getId(), keybinds::Keys::None);
            auto keyName = keybinds::keyToString(key);
            bool changed = ImGui::Button(keyName.c_str(), ImVec2(availWidth * 0.38f, 0));
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();

            auto popupName = fmt::format("##{}-popup", keybind->getId());
            if (changed) ImGui::OpenPopup(popupName.c_str());

            if (ImGui::BeginPopup(popupName.c_str())) {
                ImGuiCocos::get().setInputMode(ImGuiCocos::InputMode::Blocking);
                ImGui::Text("%s", "Press any key to change the keybind...");
                ImGui::Separator();

                ImGui::Text("%s", "Press ESC to cancel.");

                if (keybinds::isKeyDown(keybinds::Keys::Escape)) {
                    ImGui::CloseCurrentPopup();
                } else {
                    auto from = keybinds::Keys::A;
                    auto to = keybinds::Keys::LastKey;
                    for (auto i = from; i < to; i++) {
                        if (keybinds::isKeyDown(i)) {
                            config::set(keybind->getId(), i);
                            keybind->triggerCallback(i);
                            ImGui::CloseCurrentPopup();
                            break;
                        }
                    }
                }

                ImGui::EndPopup();
            }

            bool deleteClicked = false;
            if (canDelete) {
                ImGui::SameLine(0, 0);
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
                deleteClicked = ImGui::Button("X", ImVec2(availWidth * 0.1f, 0));
                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar();
                if (deleteClicked) {
                    config::set(keybind->getId(), keybinds::Keys::None);
                    keybind->triggerCallback(keybinds::Keys::None);
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        void visit(LabelSettingsComponent* labelSettings) override {
            auto& style = ImGui::GetStyle();
            auto& colors = style.Colors;
            auto* settings = labelSettings->getSettings();
            ImGui::PushID(fmt::format("label-{}", settings->id).c_str());

            ImGui::PushItemWidth(-1);

            auto textColor = settings->visible ? colors[ImGuiCol_Text] : colors[ImGuiCol_TextDisabled];
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)textColor);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            auto availWidth = ImGui::GetContentRegionAvail().x;
            auto buttonSize = ImVec2(availWidth * 0.9f, 0);
            auto arrowSize = ImVec2(availWidth * 0.1f, 0);

            if (ImGui::Button(settings->name.c_str(), buttonSize)) {
                settings->visible = !settings->visible;
                labelSettings->triggerEditCallback();
            }

            ImGui::SameLine(0, 0);

            ImGui::PopStyleVar(2);
            bool openPopup = ImGui::Button(fmt::format("##open_label_{}", settings->id).c_str(), arrowSize);
            ImGui::PopItemWidth();
            ImGui::PopStyleColor(4);

            auto scale = 1.f;
            auto top = ImGui::GetItemRectMin().y + (4.5 * scale);
            auto bottom = ImGui::GetItemRectMax().y - (4.5 * scale);
            auto right = ImGui::GetItemRectMax().x - (4.5 * scale);
            auto side = bottom - top;
            auto left = right - side;
            auto triangleColor = colors[ImGuiCol_TextDisabled];
            triangleColor.w *= ImGui::GetStyle().Alpha;
            ImGui::GetWindowDrawList()->AddTriangleFilled(
                ImVec2(right, top),
                ImVec2(left, bottom),
                ImVec2(right, bottom),
                ImGui::ColorConvertFloat4ToU32(triangleColor)
            );

            std::string popupName = fmt::format("##label-settings-{}", settings->id);
            if (openPopup)
                ImGui::OpenPopup(popupName.c_str());

            ImGui::SetNextWindowSizeConstraints(ImVec2(240, 0), ImVec2(FLT_MAX, FLT_MAX));
            if (ImGui::BeginPopup(popupName.c_str())) {
                auto& name = settings->name;
                if (ImGui::InputText("Name", &name)) {
                    settings->name = name;
                    labelSettings->triggerEditCallback();
                }

                auto& text = settings->text;
                if (ImGui::InputText("Text", &text)) {
                    settings->text = text;
                    labelSettings->triggerEditCallback();
                }

                int32_t currentFont = labels::getFontIndex(settings->font);
                if (ImGui::Combo("Font", &currentFont, labels::fontNames.data(), labels::fontNames.size())) {
                    settings->font = labels::fontFiles[currentFont];
                    labelSettings->triggerEditCallback();
                }

                auto labelScale = settings->scale;
                if (ImGui::InputFloat("Scale", &labelScale, 0.f, 0.f, "%.2f")) {
                    settings->scale = std::clamp(labelScale, 0.0f, 10.0f);
                    labelSettings->triggerEditCallback();
                }

                auto& color = settings->color;
                if (ImGui::ColorEdit3("Color", color.data(), ImGuiColorEditFlags_NoInputs)) {
                    settings->color = color;
                    labelSettings->triggerEditCallback();
                }

                auto opacity = settings->color.a;
                if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
                    settings->color.a = opacity;
                    labelSettings->triggerEditCallback();
                }

                auto alignment = settings->alignment;
                if (ImGui::Combo("Alignment", (int*)&alignment, labels::alignmentNames.data(), labels::alignmentNames.size())) {
                    settings->alignment = alignment;
                    labelSettings->triggerEditCallback();
                }

                if (ImGui::Button("Delete")) {
                    labelSettings->triggerDeleteCallback();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::PopID();
        }
    };
}
