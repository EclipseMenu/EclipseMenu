#include "theme.hpp"

#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/theming/manager.hpp>
#include <misc/cpp/imgui_stdlib.h>

std::pair<std::string, float> truncateString(std::string str, float availWidth, bool canDelete = false)
{
    auto labelMaxWidth = availWidth * (canDelete ? 0.5f : 0.6f);
    auto labelSize = ImGui::CalcTextSize(str.c_str());

    if (labelSize.x > labelMaxWidth) {
        auto labelEnd = 0;

        while (labelEnd != str.size()) {
            auto labelStr = fmt::format("{}...", str.substr(0, labelEnd));
            auto newSize = ImGui::CalcTextSize(labelStr.c_str());

            if (newSize.x > labelMaxWidth - 20)
                break;

            labelEnd++;
        }

        return {
            fmt::format("{}...", str.substr(0, labelEnd)),
            labelMaxWidth
        };    
    } else {
        return { str, labelMaxWidth };
    }
}

namespace eclipse::gui::imgui {

    std::vector<std::string> THEME_NAMES = {
        "ImGui", "MegaHack", "MegaOverlay"
    };

    void Theme::visit(const std::shared_ptr<Component>& component) const {

#define CASE(x) case ComponentType::x: \
    this->visit##x(std::static_pointer_cast<x##Component>(component)); break;

        ImGui::PushID(component->getId().c_str());
        switch (component->getType()) {
            default: break;
            CASE(Label); CASE(Toggle);
            CASE(RadioButton); CASE(Combo);
            CASE(Slider); CASE(InputFloat);
            CASE(InputInt); CASE(FloatToggle);
            CASE(InputText); CASE(Color);
            CASE(Button); CASE(Keybind);
            CASE(LabelSettings); CASE(FilesystemCombo);
            CASE(IntToggle);
        }
        ImGui::PopID();

#undef CASE
    }

    void Theme::handleTooltip(const std::string &text) {
        if (text.empty()) return;
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImVec2 pos = ImGui::GetMousePos();

            auto tm = ThemeManager::get();
            auto size = ImGui::CalcTextSize(text.c_str());
            size.x /= tm->getGlobalScale();
            auto screenSize = ImGui::GetIO().DisplaySize;

            constexpr float padding = 10.f;
            pos.x += padding;
            pos.y += padding;

            if (pos.x + size.x > screenSize.x)
                pos.x = screenSize.x - size.x - padding;
            if (pos.y + size.y > screenSize.y)
                pos.y = screenSize.y - size.y - padding;

            ImGui::SetNextWindowPos(pos);
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getForegroundColor()));
            ImGui::SetTooltip("%s", text.c_str());
            ImGui::PopStyleColor();
        }
    }

    void Theme::handleKeybindMenu(std::string_view id) {
        auto popupId = fmt::format("##context-menu-{}", id);
        if (ImGui::IsItemClicked(1) || (ImGui::IsItemClicked(0) && ImGui::GetIO().KeyShift)) {
            ImGui::OpenPopup(popupId.c_str());
        }

        if (ImGui::BeginPopup(popupId.c_str())) {
            auto keybinds = keybinds::Manager::get();
            auto keybind = keybinds->getKeybind(id);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

            if (keybind.has_value()) {
                auto& keybindRef = keybind->get();

                if (!keybindRef.isInitialized() && ImGui::MenuItem("Add keybind")) {
                    keybinds->setKeybindState(id, true);
                } else if (keybindRef.isInitialized() && ImGui::MenuItem("Remove keybind")) {
                    keybinds->setKeybindState(id, false);
                }
            }
            ImGui::PopStyleColor();

            ImGui::EndPopup();
        }
    }

    void Theme::init() {
        auto& style = ImGui::GetStyle();
        style.WindowMenuButtonPosition = ImGuiDir_Left;
    }

    void Theme::update() {
        auto tm = ThemeManager::get();
        auto &style = ImGui::GetStyle();

        ImGui::GetIO().FontGlobalScale = tm->getGlobalScale() * INV_DEFAULT_SCALE;

        // Sizes
        style.WindowPadding = ImVec2(tm->getWindowPadding(), tm->getWindowPadding());
        style.WindowRounding = tm->getWindowRounding();
        style.FramePadding = ImVec2(tm->getFramePadding(), tm->getFramePadding());
        style.FrameRounding = tm->getFrameRounding();
        style.PopupRounding = tm->getFrameRounding();
        style.ItemSpacing = ImVec2(tm->getHorizontalSpacing(), tm->getVerticalSpacing());
        style.ItemInnerSpacing = ImVec2(tm->getHorizontalInnerSpacing(), tm->getVerticalInnerSpacing());
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.IndentSpacing = tm->getIndentSpacing();
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;
        style.WindowBorderSize = tm->getBorderSize();
        style.WindowMinSize = ImVec2(32, 32);
        style.DisplayWindowPadding = ImVec2(0, 0);
        style.ScaleAllSizes(tm->getGlobalScale());

        // Base colors
        auto& colors = style.Colors;
        colors[ImGuiCol_WindowBg] = tm->getBackgroundColor();
        colors[ImGuiCol_PopupBg] = tm->getBackgroundColor();
        colors[ImGuiCol_Button] = tm->getButtonBackgroundColor();
        colors[ImGuiCol_ButtonHovered] = tm->getButtonHoveredBackground();
        colors[ImGuiCol_ButtonActive] = tm->getButtonHoveredBackground();
        colors[ImGuiCol_Border] = tm->getBorderColor();
        colors[ImGuiCol_Text] = tm->getForegroundColor();
        colors[ImGuiCol_TitleBg] = tm->getTitleBackgroundColor();
        colors[ImGuiCol_TitleBgActive] = tm->getTitleBackgroundColor();
        colors[ImGuiCol_TitleBgCollapsed] = tm->getTitleBackgroundColor();
        colors[ImGuiCol_CheckMark] = tm->getCheckboxCheckmarkColor();
        colors[ImGuiCol_FrameBg] = tm->getFrameBackground();
        colors[ImGuiCol_FrameBgHovered] = tm->getFrameBackground();
        colors[ImGuiCol_FrameBgActive] = tm->getFrameBackground();
    }

    bool Theme::beginWindow(const std::string &title) {
        ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
        auto tm = ThemeManager::get();
        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getTitleForegroundColor()));
        ImGui::PushFont(ImGuiRenderer::get()->getFontManager().getFont().get());
        bool open = ImGui::Begin(title.c_str(), nullptr, flags);
        ImGui::PopStyleColor();

        return open;
    }

    void Theme::endWindow() {
        ImGui::PopFont();
        ImGui::End();
    }

    void Theme::visitLabel(const std::shared_ptr<LabelComponent>& label) const {
        if (label->getTitle().empty()) return; // skip empty labels
        if (label->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getForegroundColor()));
        ImGui::TextWrapped("%s", label->getTitle().c_str());
        ImGui::PopStyleColor();
    }

    void Theme::visitToggle(const std::shared_ptr<ToggleComponent>& toggle) const {
        auto tm = ThemeManager::get();

        bool toggled = false;
        bool value = toggle->getValue();

        if (auto options = toggle->getOptions().lock()) {
            toggled = this->checkboxWithSettings(toggle->getTitle(), value, toggle->isSearchedFor(), [this, options] {
                for (auto& comp : options->getComponents())
                    this->visit(comp);
            }, [toggle] {
                handleTooltip(toggle->getDescription());
                if (toggle->hasKeybind())
                    handleKeybindMenu(toggle->getId());
            });
        } else {
            toggled = this->checkbox(toggle->getTitle(), value, toggle->isSearchedFor(), [toggle] {
                handleTooltip(toggle->getDescription());
                if (toggle->hasKeybind())
                    handleKeybindMenu(toggle->getId());
            });
        }

        if (toggled) {
            toggle->setValue(value);
            toggle->triggerCallback(value);
        }
    }

    void Theme::visitRadioButton(const std::shared_ptr<RadioButtonComponent>& radio) const {
        int value = radio->getValue();

        if (radio->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::PushStyleColor(ImGuiCol_CheckMark, static_cast<ImVec4>(ThemeManager::get()->getCheckboxCheckmarkColor()));
        if (ImGui::RadioButton(radio->getTitle().c_str(), &value, radio->getChoice())) {
            radio->setValue(value);
            radio->triggerCallback(value);
        }
        ImGui::PopStyleColor();

        if (radio->isSearchedFor())
            ImGui::PopStyleColor();

        handleTooltip(radio->getDescription());
        if (radio->hasKeybind())
            handleKeybindMenu(fmt::format("{}-{}", radio->getId(), radio->getChoice()));
    }

    void Theme::visitCombo(const std::shared_ptr<ComboComponent>& combo) const {
        auto& items = combo->getItems();
        int value = combo->getValue();
        auto& title = combo->getTitle();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * (title.empty() ? .9f : .5f));
        if (value >= items.size()) value = 0;
        if (ImGui::BeginCombo(fmt::format("##{}", title).c_str(), items[value].c_str())) {
            for (int n = 0; n < items.size(); n++) {
                const bool is_selected = (value == n);
                if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                    combo->setValue(n);
                    combo->triggerCallback(n);
                }

                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        ImGui::SetNextItemWidth(5.f);
        ImGui::SameLine();

        if (combo->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.c_str());

        if (combo->isSearchedFor())
            ImGui::PopStyleColor();
    }

    void Theme::visitFilesystemCombo(const std::shared_ptr<FilesystemComboComponent>& combo) const {
        auto& items = combo->getItems();
        std::filesystem::path value = combo->getValue();
        auto title = combo->getTitle();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * (title.empty() ? .9f : .5f));
        if (ImGui::BeginCombo(fmt::format("##{}", title.c_str()).c_str(), value.empty() ? "None" : value.filename().stem().string().c_str())) {
            ImGui::InputText("##search", combo->getSearchBuffer());
            for (int n = 0; n < items.size(); n++) {
                std::string option = items[n].filename().stem().string();
                if(option.find(*combo->getSearchBuffer()) != std::string::npos) {
                    const bool is_selected = (value == items[n]);
                    if (ImGui::Selectable(option.c_str(), is_selected)) {
                        combo->setValue(n);
                        combo->triggerCallback(n);
                    }

                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        ImGui::SetNextItemWidth(5.f);
        ImGui::SameLine();

        if (combo->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.c_str());

        if (combo->isSearchedFor())
            ImGui::PopStyleColor();
    }

    void Theme::visitSlider(const std::shared_ptr<SliderComponent>& slider) const {
        auto value = slider->getValue();

        if (slider->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::SliderFloat(slider->getTitle().c_str(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
            slider->setValue(value);
            slider->triggerCallback(value);
        }

        if (slider->isSearchedFor())
            ImGui::PopStyleColor();

        handleTooltip(slider->getDescription());
        ImGui::PopItemWidth();
    }

    void Theme::visitInputFloat(const std::shared_ptr<InputFloatComponent> &inputFloat) const {
        auto value = inputFloat->getValue();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputFloat(fmt::format("##{}", inputFloat->getTitle()).c_str(), &value, 0, 0, inputFloat->getFormat().c_str())) {
            value = std::clamp(value, inputFloat->getMin(), inputFloat->getMax());
            inputFloat->setValue(value);
            inputFloat->triggerCallback(value);
        }

        ImGui::SameLine();

        if (inputFloat->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", inputFloat->getTitle().c_str());

        if (inputFloat->isSearchedFor())
            ImGui::PopStyleColor();

        handleTooltip(inputFloat->getDescription());
        ImGui::PopItemWidth();
    }

    void Theme::visitInputInt(const std::shared_ptr<InputIntComponent> &inputInt) const {
        auto value = inputInt->getValue();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputInt(fmt::format("##{}", inputInt->getTitle()).c_str(), &value, 0, 0)) {
            value = std::clamp(value, inputInt->getMin(), inputInt->getMax());
            inputInt->setValue(value);
            inputInt->triggerCallback(value);
        }

        ImGui::SameLine();

        if (inputInt->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", inputInt->getTitle().c_str());

        if (inputInt->isSearchedFor())
            ImGui::PopStyleColor();

        handleTooltip(inputInt->getDescription());
        ImGui::PopItemWidth();
    }

    void Theme::visitIntToggle(const std::shared_ptr<IntToggleComponent>& intToggle) const {
        auto value = intToggle->getValue();
        auto state = intToggle->getState();
        auto tm = ThemeManager::get();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputInt(fmt::format("##{}", intToggle->getTitle()).c_str(), &value, 0, 0)) {
            value = std::clamp(value, intToggle->getMin(), intToggle->getMax());
            intToggle->setValue(value);
            intToggle->triggerCallback(value);
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, 1);

        if (this->checkbox(intToggle->getTitle(), state, intToggle->isSearchedFor(), [intToggle] {
            handleTooltip(intToggle->getDescription());
            if (intToggle->hasKeybind())
                handleKeybindMenu(intToggle->getId());
        })) {
            intToggle->setState(state);
            intToggle->triggerCallback(value);
        }
    }

    void Theme::visitFloatToggle(const std::shared_ptr<FloatToggleComponent>& floatToggle) const {
        auto value = floatToggle->getValue();
        auto state = floatToggle->getState();
        auto tm = ThemeManager::get();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputFloat(fmt::format("##{}", floatToggle->getTitle()).c_str(), &value, 0, 0, floatToggle->getFormat().c_str())) {
            value = std::clamp(value, floatToggle->getMin(), floatToggle->getMax());
            floatToggle->setValue(value);
            floatToggle->triggerCallback(value);
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, 1);

        if (this->checkbox(floatToggle->getTitle(), state, floatToggle->isSearchedFor(), [floatToggle] {
            handleTooltip(floatToggle->getDescription());
            if (floatToggle->hasKeybind())
                handleKeybindMenu(floatToggle->getId());
        })) {
            floatToggle->setState(state);
            floatToggle->triggerCallback(value);
        }
    }

    void Theme::visitInputText(const std::shared_ptr<InputTextComponent>& inputText) const {
        auto value = inputText->getValue();

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        if (ImGui::InputText(fmt::format("##{}", inputText->getTitle()).c_str(), &value)) {
            inputText->setValue(value);
            inputText->triggerCallback(value);
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();

        if (inputText->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        // ImGui::PushTextWrapPos(0.0f);
        ImGui::Text("%s", inputText->getTitle().c_str());
        // ImGui::PopTextWrapPos();

        if (inputText->isSearchedFor())
            ImGui::PopStyleColor();

        handleTooltip(inputText->getDescription());
    }

    void Theme::visitColor(const std::shared_ptr<ColorComponent>& color) const {
        auto value = color->getValue();
        bool changed = false;

        if (color->hasOpacity()) {
            changed = ImGui::ColorEdit4(fmt::format("##{}", color->getTitle()).c_str(), value.data(), ImGuiColorEditFlags_NoInputs);
        } else {
            changed = ImGui::ColorEdit3(fmt::format("##{}", color->getTitle()).c_str(), value.data(), ImGuiColorEditFlags_NoInputs);
        }

        ImGui::SameLine();

        if (color->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", color->getTitle().c_str());

        if (color->isSearchedFor())
            ImGui::PopStyleColor();

        if (changed) {
            color->setValue(value);
            color->triggerCallback(value);
        }
        handleTooltip(color->getDescription());
    }

    void Theme::visitButton(const std::shared_ptr<ButtonComponent>& button) const {
        if (this->button(button->getTitle(), button->isSearchedFor())) {
            button->triggerCallback();
        }

        handleTooltip(button->getDescription());
        if (button->hasKeybind())
            handleKeybindMenu(fmt::format("button.{}", button->getId()));
    }

    void Theme::visitKeybind(const std::shared_ptr<KeybindComponent>& keybind) const {
        auto& title = keybind->getTitle();
        auto canDelete = keybind->canDelete();

        ImGui::PushID(title.c_str());
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        if (keybind->isSearchedFor())
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        auto availWidth = ImGui::GetContentRegionAvail().x;

        // truncate title if it's too long
        const auto& [truncatedLabel, maxWidth] = truncateString(title, availWidth, canDelete);
        if (truncatedLabel == title) {
            ImGui::Button(title.c_str(), ImVec2(maxWidth, 0));
        } else {
            ImGui::Button(truncatedLabel.c_str(), ImVec2(maxWidth, 0));
            handleTooltip(title);
        }

        ImGui::SameLine(0, 2);

        ImGui::PopStyleColor(keybind->isSearchedFor() ? 4 : 3);
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

        if (canDelete) {
            ImGui::SameLine(0, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.07f, 0.07f, 0.07f, 0.5f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.04f, 0.04f, 0.04f, 0.5f));
            bool deleteClicked = ImGui::Button("X", ImVec2(availWidth * 0.1f, 0));
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

    void Theme::visitLabelSettings(const std::shared_ptr<LabelSettingsComponent>& labelSettings) const {
        auto* settings = labelSettings->getSettings();

        if (this->checkboxWithSettings(settings->name, settings->visible, labelSettings->isSearchedFor(), [this, settings, labelSettings] {
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
            if (ImGui::Combo("Alignment", reinterpret_cast<int*>(&alignment), labels::alignmentNames.data(), labels::alignmentNames.size())) {
                settings->alignment = alignment;
                labelSettings->triggerEditCallback();
            }

            if (this->button("Delete", false)) {
                labelSettings->triggerDeleteCallback();
                ImGui::CloseCurrentPopup();
            }

            if (this->button("Move Up", false)) {
                labelSettings->triggerMoveCallback(true);
            }
            if (this->button("Move Down", false)) {
                labelSettings->triggerMoveCallback(false);
            }

        }, [labelSettings, settings] {
            if (labelSettings->hasKeybind())
                handleKeybindMenu(fmt::format("label.{}", settings->id));
        }, fmt::format("label-setting-{}", settings->id)))
            labelSettings->triggerEditCallback();
    }

    bool Theme::checkbox(const std::string &label, bool &value, bool isSearchedFor, const std::function<void()> &postDraw) const {
        auto tm = ThemeManager::get();

        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(isSearchedFor ? tm->getSearchedColor() : tm->getCheckboxForegroundColor()));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, static_cast<ImVec4>(tm->getCheckboxCheckmarkColor()));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, static_cast<ImVec4>(tm->getCheckboxBackgroundColor()));

        bool result = ImGui::Checkbox(label.c_str(), &value);
        postDraw();

        ImGui::PopStyleColor(3);
        return result;
    }

    bool Theme::checkboxWithSettings(const std::string &label, bool &value,
                                     bool isSearchedFor,
                                     const std::function<void()> &callback,
                                     const std::function<void()> &postDraw,
                                     const std::string& popupId) const {
        auto tm = ThemeManager::get();

        bool result = this->checkbox(label, value, isSearchedFor, postDraw);

        ImGui::PushItemWidth(-1);
        auto availWidth = ImGui::GetContentRegionAvail().x;
        auto arrowSize = ImVec2(availWidth * 0.18f, 0);
        ImGui::SameLine(availWidth - (arrowSize.x / 2.f), 0);
        ImGui::SetNextItemWidth(arrowSize.x);
        bool openPopup = ImGui::ArrowButton(fmt::format("##open_{}", label).c_str(), ImGuiDir_Right);
        ImGui::PopItemWidth();

        std::string popupName = popupId.empty() ? fmt::format("##{}", label) : popupId;
        if (openPopup)
            ImGui::OpenPopup(popupName.c_str());

        ImGui::SetNextWindowSizeConstraints(ImVec2(240 * tm->getGlobalScale(), 0), ImVec2(FLT_MAX, FLT_MAX));
        if (ImGui::BeginPopup(popupName.c_str(), ImGuiWindowFlags_NoMove)) {
            callback();
            ImGui::EndPopup();
        }

        return result;
    }

    bool Theme::button(const std::string &text, bool isSearchedFor) const {
        ImGui::PushItemWidth(-1);

        auto tm = ThemeManager::get();

        if (isSearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getSearchedColor()));

        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(tm->getButtonBackgroundColor()));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(tm->getButtonHoveredBackground()));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(tm->getButtonActivatedBackground()));

        bool pressed = ImGui::Button(text.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));

        ImGui::PopStyleColor(isSearchedFor ? 4 : 3);
        ImGui::PopItemWidth();

        return pressed;
    }
}


