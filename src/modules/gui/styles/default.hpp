#pragma once

#include <modules/gui/gui.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <modules/config/config.hpp>
#include <imgui-cocos.hpp>

namespace eclipse::gui::imgui {
    class DefaultStyle : public Style {
    public:
        void visit(LabelComponent* label) override {
            ImGui::TextWrapped("%s", label->getTitle().c_str());
        }

        void visit(ToggleComponent* checkbox) override {
            bool value = checkbox->getValue();
            if (ImGui::Checkbox(checkbox->getTitle().c_str(), &value)) {
                checkbox->setValue(value);
                checkbox->triggerCallback(value);
            }

            if (checkbox->hasKeybind()) {
                // Open context menu on either Right click or Shift+Click
                if (ImGui::IsItemClicked(1) || (ImGui::IsItemClicked(0) && ImGui::GetIO().KeyShift)) {
                    ImGui::OpenPopup(fmt::format("##context-menu-{}", checkbox->getId()).c_str());
                }

                if (ImGui::BeginPopup(fmt::format("##context-menu-{}", checkbox->getId()).c_str())) {
                    auto keybinds = keybinds::Manager::get();
                    auto keybind = keybinds->getKeybind(checkbox->getId());

                    if (keybind.has_value()) {
                        auto& keybindRef = keybind->get();

                        if (!keybindRef.isInitialized() && ImGui::MenuItem("Add keybind"))
                            keybinds->setKeybindState(checkbox->getId(), true);
                        else if (keybindRef.isInitialized() && ImGui::MenuItem("Remove keybind"))
                            keybinds->setKeybindState(checkbox->getId(), false);
                    }

                    ImGui::EndPopup();
                }
            }

            if (!checkbox->getDescription().empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(checkbox->getDescription().c_str());
                    ImGui::EndTooltip();
                }
            }

            if (auto options = checkbox->getOptions().lock()) {
                ImGui::PushItemWidth(-1);
                auto availWidth = ImGui::GetContentRegionAvail().x;
                auto buttonSize = ImVec2(availWidth * 0.885f, 0);
                auto arrowSize = ImVec2(availWidth * 0.115f, 0);
                ImGui::SameLine(availWidth - (arrowSize.x / 2.f), 0);
                ImGui::SetNextItemWidth(arrowSize.x);
                bool openPopup = ImGui::ArrowButton(fmt::format("##open_{}", checkbox->getTitle()).c_str(), ImGuiDir_Right);
                ImGui::PopItemWidth();

                std::string popupName = fmt::format("##{}", checkbox->getTitle().c_str());
                if (openPopup)
                    ImGui::OpenPopup(popupName.c_str());

                ImGui::SetNextWindowSizeConstraints(ImVec2(240, 0), ImVec2(FLT_MAX, FLT_MAX));
                if (ImGui::BeginPopup(popupName.c_str(), ImGuiWindowFlags_NoMove)) {
                    for (auto comp : options->getComponents())
                        Style::visit(comp.get());

                    ImGui::EndPopup();
                }
            }
        }

        void visit(SliderComponent* slider) override {
            auto value = config::get<float>(slider->getId(), 0.f);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);
            if (ImGui::SliderFloat(slider->getTitle().c_str(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
                config::set(slider->getId(), value);
                slider->triggerCallback(value);
            }
            ImGui::PopItemWidth();
        }

        void visit(InputFloatComponent* inputFloat) override {
            auto value = config::get<float>(inputFloat->getId(), 0.f);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::InputFloat(inputFloat->getTitle().c_str(), &value, 0, 0, inputFloat->getFormat().c_str())) {
                value = std::clamp(value, inputFloat->getMin(), inputFloat->getMax());
                config::set(inputFloat->getId(), value);
                inputFloat->triggerCallback(value);
            }
            ImGui::PopItemWidth();
        }

        void visit(InputIntComponent* inputInt) override {
            auto value = config::get<int>(inputInt->getId(), 0);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::InputInt(inputInt->getTitle().c_str(), &value, 0, 0)) {
                value = std::clamp(value, inputInt->getMin(), inputInt->getMax());
                config::set(inputInt->getId(), value);
                inputInt->triggerCallback(value);
            }
            ImGui::PopItemWidth();
        }

        void visit(FloatToggleComponent* floatToggle) override {
            auto value = config::get<float>(floatToggle->getId(), 0.0f);
            bool toggle = config::get<bool>(fmt::format("{}.toggle", floatToggle->getId()), false);

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
            if (ImGui::InputFloat(fmt::format("##{}", floatToggle->getTitle()).c_str(), &value, 0, 0, floatToggle->getFormat().c_str())) {
                value = std::clamp(value, floatToggle->getMin(), floatToggle->getMax());
                config::set(floatToggle->getId(), value);
                floatToggle->triggerCallback(value);
            }
            ImGui::PopItemWidth();

            ImGui::SameLine();
            if (ImGui::Checkbox(floatToggle->getTitle().c_str(), &toggle)) {
                config::set(fmt::format("{}.toggle", floatToggle->getId()), toggle);
                floatToggle->triggerCallback();
            }
            if (!floatToggle->getDescription().empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(floatToggle->getDescription().c_str());
                    ImGui::EndTooltip();
                }
            }
        }

        void visit(RadioButtonComponent* radioButton) override {
            int value = config::get<int>(radioButton->getId(), radioButton->getValue());
            if (ImGui::RadioButton(radioButton->getTitle().c_str(), &value, radioButton->getValue())) {
                config::set(radioButton->getId(), value);
                radioButton->triggerCallback(value);
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
            ImGui::PopItemWidth();
        }

        void visit(ButtonComponent* button) override {
            if (ImGui::Button(button->getTitle().c_str()))
                button->triggerCallback();
        }

        void visit(ColorComponent* color) override {
            auto value = config::get<gui::Color>(color->getId(), {1, 1, 1, 1});
            if (ImGui::ColorEdit3(color->getTitle().c_str(), value.data(), ImGuiColorEditFlags_NoInputs)) {
                config::set(color->getId(), value);
                color->triggerCallback(value);
            }
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
                // TODO: Add a tooltip on hover
            } else {
                ImGui::Button(title.c_str(), ImVec2(labelMaxWidth, 0));
            }

            ImGui::SameLine(.0f, .0f);

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));

            auto key = config::get<keybinds::Keys>(keybind->getId(), keybinds::Keys::None);
            auto keyName = keybinds::keyToString(key);
            bool changed = ImGui::Button(keyName.c_str(), ImVec2(availWidth * 0.4f, 0));
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
    };

}
