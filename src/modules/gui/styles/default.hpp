#pragma once

#include <modules/gui/gui.hpp>
#include <misc/cpp/imgui_stdlib.h>

namespace eclipse::gui::imgui {
    class DefaultStyle : public Style {
    public:
        void visit(Component* component) override {
            if (auto* label = dynamic_cast<LabelComponent*>(component)) {
                ImGui::TextWrapped("%s", label->getTitle().c_str());
            } else if (auto* checkbox = dynamic_cast<ToggleComponent*>(component)) {
                bool value = checkbox->getValue();
                if (ImGui::Checkbox(checkbox->getTitle().c_str(), &value)) {
                    checkbox->setValue(value);
                    checkbox->triggerCallback(value);
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
                if (checkbox->getOptions()) {
                    ImGui::PushItemWidth(-1);
                    auto availWidth = ImGui::GetContentRegionAvail().x;
                    auto buttonSize = ImVec2(availWidth * 0.885f, 0);
                    auto arrowSize = ImVec2(availWidth * 0.115f, 0);
                    ImGui::SameLine(availWidth - (arrowSize.x / 2.f), 0);
                    ImGui::SetNextItemWidth(arrowSize.x);
                    bool openPopup = ImGui::ArrowButton((std::string("##open_") + checkbox->getTitle()).c_str(), ImGuiDir_Right);
                    ImGui::PopItemWidth();

                    std::string popupName = std::string("##") + checkbox->getTitle().c_str();
                    if (openPopup)
                        ImGui::OpenPopup(popupName.c_str());

                    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 0), ImVec2(FLT_MAX, FLT_MAX));
                    if (ImGui::BeginPopup(popupName.c_str(), ImGuiWindowFlags_NoMove)) {
                        for (Component* comp : checkbox->getOptions()->getComponents()) {
                            visit(comp);
                        }
                        ImGui::EndPopup();
                    }
                }
            } else if (auto* slider = dynamic_cast<SliderComponent*>(component)) {
                auto value = config::get<float>(slider->getId(), 0.0f);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);
                if (ImGui::SliderFloat(slider->getTitle().c_str(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
                    config::set(slider->getId(), value);
                    slider->triggerCallback(value);
                }
                ImGui::PopItemWidth();
            } else if (auto* inputfloat = dynamic_cast<InputFloatComponent*>(component)) {
                auto value = config::get<float>(inputfloat->getId(), 0.0f);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
                if (ImGui::InputFloat(inputfloat->getTitle().c_str(), &value, 0, 0, inputfloat->getFormat().c_str())) {

                    if(value < inputfloat->getMin()) value = inputfloat->getMin();
                    if(value > inputfloat->getMax()) value = inputfloat->getMax();

                    config::set(inputfloat->getId(), value);
                    inputfloat->triggerCallback(value);
                }
                ImGui::PopItemWidth();
            } else if (auto* inputInt = dynamic_cast<InputIntComponent*>(component)) {
                int value = config::get<int>(inputInt->getId(), 0);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
                if (ImGui::InputInt(inputInt->getTitle().c_str(), &value, 0, 0)) {

                    if(value < inputInt->getMin()) value = inputInt->getMin();
                    if(value > inputInt->getMax()) value = inputInt->getMax();

                    config::set(inputInt->getId(), value);
                    inputInt->triggerCallback(value);
                }
                ImGui::PopItemWidth();
            } else if (auto* floattoggle = dynamic_cast<FloatToggleComponent*>(component)) {
                float value = config::get<float>(floattoggle->getId(), 0.0f);
                bool toggle = config::get<bool>(floattoggle->getId() + ".toggle", false);

                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
                if (ImGui::InputFloat(("##" + floattoggle->getTitle()).c_str(), &value, 0, 0, floattoggle->getFormat().c_str())) {
                    if(value < floattoggle->getMin()) value = floattoggle->getMin();
                    if(value > floattoggle->getMax()) value = floattoggle->getMax();

                    config::set(floattoggle->getId(), value);
                    floattoggle->triggerCallback(value);
                }
                ImGui::PopItemWidth();

                ImGui::SameLine();
                if (ImGui::Checkbox(floattoggle->getTitle().c_str(), &toggle)) {
                    config::set(floattoggle->getId() + ".toggle", toggle);
                    floattoggle->triggerCallback();
                }

            } else if (auto* radio = dynamic_cast<RadioButtonComponent*>(component)) {
                int value = config::get<int>(radio->getId(), radio->getValue());
                if (ImGui::RadioButton(radio->getTitle().c_str(), &value, radio->getValue())) {
                    config::set(radio->getId(), value);
                    radio->triggerCallback(value);
                }
            } else if (auto* combo = dynamic_cast<ComboComponent*>(component)) {
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
            } else if (auto* inputtext = dynamic_cast<InputTextComponent*>(component)) {
                auto value = config::get<std::string>(inputtext->getId(), "");
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                if (ImGui::InputText(inputtext->getTitle().c_str(), &value)) {
                    config::set(inputtext->getId(), value);
                    inputtext->triggerCallback(value);
                }
                ImGui::PopItemWidth();
            } else if (auto* button = dynamic_cast<ButtonComponent*>(component)) {
                if (ImGui::Button(button->getTitle().c_str())) {
                    button->triggerCallback();
                }
            }
            else if (auto* color = dynamic_cast<ColorComponent*>(component)) {
                auto value = config::get<gui::Color>(color->getId(), {1, 1, 1, 1});
                if(ImGui::ColorEdit3(color->getTitle().c_str(), (float*)&value, ImGuiColorEditFlags_NoInputs)) {
                    config::set(color->getId(), value);
                    color->triggerCallback(value);
                }
            } else if (auto* keybind = dynamic_cast<KeybindComponent*>(component)) {
                auto& title = keybind->getTitle();
                auto canDelete = keybind->canDelete();

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

                ImGui::SameLine(0, 0);

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

                    ImGui::Text("%s", "Press ESC to clear the cancel.");

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
            }
        };
    };
}