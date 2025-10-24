#include "theme.hpp"

#include <imgui-cocos.hpp>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/i18n/translations.hpp>
#include <modules/labels/setting.hpp>
#include <modules/labels/variables.hpp>

#include <modules/gui/components/button.hpp>
#include <modules/gui/components/color.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/gui/components/filesystem-combo.hpp>
#include <modules/gui/components/float-toggle.hpp>
#include <modules/gui/components/input-float.hpp>
#include <modules/gui/components/input-int.hpp>
#include <modules/gui/components/input-text.hpp>
#include <modules/gui/components/int-toggle.hpp>
#include <modules/gui/components/keybind.hpp>
#include <modules/gui/components/label-settings.hpp>
#include <modules/gui/components/label.hpp>
#include <modules/gui/components/radio.hpp>
#include <modules/gui/components/slider.hpp>
#include <modules/gui/components/toggle.hpp>

std::pair<std::string, float> truncateString(std::string_view str, float availWidth, bool canDelete = false) {
    auto labelMaxWidth = availWidth * (canDelete ? 0.5f : 0.6f);
    auto labelSize = ImGui::CalcTextSize(str.data());

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
    }

    return { std::string(str), labelMaxWidth };
}

namespace eclipse::gui::imgui {

    std::vector<std::string> THEME_NAMES = {
        "ImGui", "MegaHack", "MegaOverlay", "Gruvbox", "OpenHack"
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

    void Theme::handleTooltip(const std::string& text) {
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
            ImGui::SetTooltip("%s", i18n::get(text).data());
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

                if (!keybindRef.isInitialized() && ImGui::MenuItem(i18n::get("keybinds.add").data())) {
                    keybinds->setKeybindState(id, true);
                } else if (keybindRef.isInitialized() && ImGui::MenuItem(i18n::get("keybinds.remove").data())) {
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
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
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

    bool Theme::beginWindow(const std::string& title) {
        ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
        auto tm = ThemeManager::get();
        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getTitleForegroundColor()));
        ImGui::PushFont(ImGuiRenderer::get()->getFontManager().getFont().get());
        bool open = ImGui::Begin(i18n::get(title).data(), nullptr, flags);
        ImGui::PopStyleColor();

        return open;
    }

    void Theme::endWindow() {
        ImGui::PopFont();
        ImGui::End();
    }

    void Theme::visitLabel(const std::shared_ptr<LabelComponent>& label) const {
        if (label->getTitle().empty()) return; // skip empty labels
        if (label->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getForegroundColor()));
        ImGui::TextWrapped("%s", i18n::get(label->getTitle()).data());
        ImGui::PopStyleColor();
    }

    void Theme::visitToggle(const std::shared_ptr<ToggleComponent>& toggle) const {
        auto tm = ThemeManager::get();

        bool toggled = false;
        bool value = toggle->getValue();
        auto title = i18n::get_(toggle->getTitle());

        if (auto options = toggle->getOptions().lock()) {
            toggled = this->checkboxWithSettings(title, value, toggle->getFlags() & ComponentFlags::SearchedFor, [this, options] {
                for (auto& comp : options->getComponents())
                    this->visit(comp);
            }, [toggle] {
                handleTooltip(toggle->getDescription());
                if (toggle->hasKeybind())
                    handleKeybindMenu(toggle->getId());
            });
        } else {
            toggled = this->checkbox(title, value, toggle->getFlags() & ComponentFlags::SearchedFor, [toggle] {
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

        if (radio->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::PushStyleColor(ImGuiCol_CheckMark, static_cast<ImVec4>(ThemeManager::get()->getCheckboxCheckmarkColor()));
        if (ImGui::RadioButton(i18n::get(radio->getTitle()).data(), &value, radio->getChoice())) {
            radio->setValue(value);
            radio->triggerCallback(value);
        }
        ImGui::PopStyleColor();

        if (radio->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();

        handleTooltip(radio->getDescription());
        if (radio->hasKeybind())
            handleKeybindMenu(fmt::format("{}-{}", radio->getId(), radio->getChoice()));
    }

    void Theme::visitCombo(const std::shared_ptr<ComboComponent>& combo) const {
        auto& items = combo->getItems();
        int value = combo->getValue();
        auto title = i18n::get(combo->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * (title.empty() ? .9f : .5f));
        if (value >= items.size()) value = std::clamp(value, 0, static_cast<int>(items.size() - 1));
        auto preview = value < items.size() ? items[value] : "";
        if (ImGui::BeginCombo(fmt::format("##{}", title).c_str(), preview.c_str())) {
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

        if (combo->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.data());

        if (combo->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();
    }

    void Theme::visitFilesystemCombo(const std::shared_ptr<FilesystemComboComponent>& combo) const {
        auto& items = combo->getItems();
        std::filesystem::path value = combo->getValue();
        auto title = i18n::get(combo->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * (title.empty() ? .9f : .5f));
        if (ImGui::BeginCombo(fmt::format("##{}", title).c_str(), value.empty() ? "None" : geode::utils::string::pathToString(value.filename().stem()).c_str())) {
            ImGui::InputText("##search", combo->getSearchBuffer());
            for (int n = 0; n < items.size(); n++) {
                std::string option = geode::utils::string::pathToString(items[n].filename().stem());
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

        if (combo->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.data());

        if (combo->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();
    }

    void Theme::visitSlider(const std::shared_ptr<SliderComponent>& slider) const {
        auto value = slider->getValue();

        if (slider->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::SliderFloat(i18n::get(slider->getTitle()).data(), &value, slider->getMin(), slider->getMax(), slider->getFormat().c_str())) {
            slider->setValue(value);
            slider->triggerCallback(value);
        }

        if (slider->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();

        handleTooltip(slider->getDescription());
        ImGui::PopItemWidth();
    }

    void Theme::visitInputFloat(const std::shared_ptr<InputFloatComponent>& inputFloat) const {
        auto value = inputFloat->getValue();
        auto title = i18n::get(inputFloat->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputFloat(fmt::format("##{}", title).c_str(), &value, 0, 0, inputFloat->getFormat().c_str())) {
            value = std::clamp(value, inputFloat->getMin(), inputFloat->getMax());
            inputFloat->setValue(value);
            inputFloat->triggerCallback(value);
        }

        ImGui::SameLine();

        if (inputFloat->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.data());

        if (inputFloat->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();

        handleTooltip(inputFloat->getDescription());
        ImGui::PopItemWidth();
    }

    void Theme::visitInputInt(const std::shared_ptr<InputIntComponent>& inputInt) const {
        auto value = inputInt->getValue();
        auto title = i18n::get(inputInt->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputInt(fmt::format("##{}", title).c_str(), &value, 0, 0)) {
            value = std::clamp(value, inputInt->getMin(), inputInt->getMax());
            inputInt->setValue(value);
            inputInt->triggerCallback(value);
        }

        ImGui::SameLine();

        if (inputInt->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.data());

        if (inputInt->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();

        handleTooltip(inputInt->getDescription());
        ImGui::PopItemWidth();
    }

    void Theme::visitIntToggle(const std::shared_ptr<IntToggleComponent>& intToggle) const {
        auto value = intToggle->getValue();
        auto state = intToggle->getState();
        auto tm = ThemeManager::get();
        auto title = i18n::get_(intToggle->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputInt(fmt::format("##{}", title).c_str(), &value, 0, 0)) {
            value = std::clamp(value, intToggle->getMin(), intToggle->getMax());
            intToggle->setValue(value);
            intToggle->triggerCallback(value);
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, 1);

        if (this->checkbox(title, state, intToggle->getFlags() & ComponentFlags::SearchedFor, [intToggle] {
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
        auto title = i18n::get_(floatToggle->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.35f);
        if (ImGui::InputFloat(fmt::format("##{}", title).c_str(), &value, 0, 0, floatToggle->getFormat().c_str())) {
            value = std::clamp(value, floatToggle->getMin(), floatToggle->getMax());
            floatToggle->setValue(value);
            floatToggle->triggerCallback(value);
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, 1);

        if (this->checkbox(title, state, floatToggle->getFlags() & ComponentFlags::SearchedFor, [floatToggle] {
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
        auto title = i18n::get(inputText->getTitle());

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        if (ImGui::InputText(fmt::format("##{}", inputText->getTitle()).c_str(), &value)) {
            inputText->setValue(value);
            inputText->triggerCallback(value);
        }
        ImGui::PopItemWidth();

        if (inputText->getFlags() & ComponentFlags::StartWithKeyboardFocus && ImGuiRenderer::get()->getLayout()->canForceKeyboardFocus())
            ImGui::SetKeyboardFocusHere(-1);

        ImGui::SameLine();

        if (inputText->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        // ImGui::PushTextWrapPos(0.0f);
        ImGui::Text("%s", title.data());
        // ImGui::PopTextWrapPos();

        if (inputText->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();

        handleTooltip(inputText->getDescription());
    }

    void Theme::visitColor(const std::shared_ptr<ColorComponent>& color) const {
        auto value = color->getValue();
        bool changed = false;
        auto title = i18n::get(color->getTitle());

        if (color->hasOpacity()) {
            changed = ImGui::ColorEdit4(fmt::format("##{}", title).c_str(), value.data(), ImGuiColorEditFlags_NoInputs);
        } else {
            changed = ImGui::ColorEdit3(fmt::format("##{}", title).c_str(), value.data(), ImGuiColorEditFlags_NoInputs);
        }

        ImGui::SameLine();

        if (color->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        ImGui::TextWrapped("%s", title.data());

        if (color->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PopStyleColor();

        if (changed) {
            color->setValue(value);
            color->triggerCallback(value);
        }
        handleTooltip(color->getDescription());
    }

    void Theme::visitButton(const std::shared_ptr<ButtonComponent>& button) const {
        if (this->button(i18n::get_(button->getTitle()), button->getFlags() & ComponentFlags::SearchedFor)) {
            button->triggerCallback();
        }

        handleTooltip(button->getDescription());
        if (button->hasKeybind())
            handleKeybindMenu(fmt::format("button.{}", button->getId()));
    }

    void Theme::visitKeybind(const std::shared_ptr<KeybindComponent>& keybind) const {
        auto title = i18n::get(keybind->getTitle());
        auto canDelete = keybind->canDelete();

        ImGui::PushID(title.data());
        ImGui::PushItemWidth(-1);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

        if (keybind->getFlags() & ComponentFlags::SearchedFor)
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(ThemeManager::get()->getSearchedColor()));

        auto availWidth = ImGui::GetContentRegionAvail().x;

        // truncate title if it's too long
        const auto& [truncatedLabel, maxWidth] = truncateString(title, availWidth, canDelete);
        if (truncatedLabel == title) {
            ImGui::Button(title.data(), ImVec2(maxWidth, 0));
        } else {
            ImGui::Button(truncatedLabel.c_str(), ImVec2(maxWidth, 0));
            handleTooltip(std::string(title));
        }

        ImGui::SameLine(0, 2);

        ImGui::PopStyleColor(keybind->getFlags() & ComponentFlags::SearchedFor ? 4 : 3);
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
            ImGui::Text("%s", i18n::get("keybinds.press-key").data());
            ImGui::Separator();

            ImGui::Text("%s", i18n::get("keybinds.press-esc").data());

            if (keybinds::isKeyDown(keybinds::Keys::Escape)) {
                ImGui::CloseCurrentPopup();
            } else {
                auto from = keybinds::Keys::A;
                auto to = keybinds::Keys::LastKey;
                for (auto i = from; i < to; ++i) {
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

        if (this->checkboxWithSettings(settings->name, settings->visible, labelSettings->getFlags() & ComponentFlags::SearchedFor, [this, settings, labelSettings] {
            auto& name = settings->name;
            if (ImGui::InputText(i18n::get("labels.name").data(), &name)) {
                settings->name = name;
                labelSettings->triggerEditCallback();
            }

            // auto& text = settings->text;
            // if (ImGui::InputText(i18n::get("labels.text").data(), &text)) {
            //     settings->text = text;
            //     labelSettings->triggerEditCallback();
            // }

            ImGui::Text("%s", i18n::get("labels.text").data());
            if (ImGui::InputTextMultiline("##code-editor", &settings->text, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput)) {
                labelSettings->triggerEditCallback();
            }

            if (ImGui::CollapsingHeader(i18n::get("labels.preview").data())) {
                auto res = rift::format(settings->text, labels::VariableManager::get().getVariables());
                if (res.isOk()) {
                    ImGui::TextWrapped("%s", res.unwrap().c_str());
                } else {
                    ImGui::TextWrapped("%s", res.unwrapErr().prettyPrint().c_str());
                }
            }

            ImGui::Separator();

            int32_t currentFont = labels::getFontIndex(settings->font);
            if (ImGui::Combo(i18n::get("labels.font").data(), &currentFont, labels::fontNames.data(), labels::fontNames.size())) {
                settings->font = labels::fontFiles[currentFont];
                labelSettings->triggerEditCallback();
            }

            auto labelScale = settings->scale;
            if (ImGui::InputFloat(i18n::get("labels.scale").data(), &labelScale, 0.f, 0.f, "%.2f")) {
                settings->scale = std::clamp(labelScale, 0.0f, 10.0f);
                labelSettings->triggerEditCallback();
            }

            auto& color = settings->color;
            if (ImGui::ColorEdit3(i18n::get("labels.color").data(), color.data(), ImGuiColorEditFlags_NoInputs)) {
                settings->color = color;
                labelSettings->triggerEditCallback();
            }

            auto opacity = settings->color.a;
            if (ImGui::InputFloat(i18n::get("labels.opacity").data(), &opacity, 0.0f, 1.0f)) {
                settings->color.a = opacity;
                labelSettings->triggerEditCallback();
            }

            auto alignment = settings->alignment;
            if (ImGui::Combo(i18n::get("labels.alignment").data(), reinterpret_cast<int*>(&alignment), labels::alignmentNames.data(), labels::alignmentNames.size())) {
                settings->alignment = alignment;
                labelSettings->triggerEditCallback();
            }

            auto fontAlignment = settings->fontAlignment;
            if (ImGui::Combo(i18n::get("labels.font-alignment").data(), reinterpret_cast<int*>(&fontAlignment), labels::fontAlignmentNames.data(), labels::fontAlignmentNames.size())) {
                settings->fontAlignment = fontAlignment;
                labelSettings->triggerEditCallback();
            }

            auto absolute = settings->absolutePosition;
            if (this->checkbox(i18n::get("labels.absolute").data(), absolute, false)) {
                settings->absolutePosition = absolute;
                labelSettings->triggerEditCallback();
            }

            if (absolute) {
                auto offset = settings->offset;
                if (ImGui::InputFloat2(i18n::get("labels.offset").data(), reinterpret_cast<float*>(&offset), "%.2f")) {
                    settings->offset = offset;
                    labelSettings->triggerEditCallback();
                }
            }

            auto& events = settings->events;
            if (ImGui::CollapsingHeader(i18n::get("labels.events").data())) {
                std::vector<int> toDeleteIndices;

                for (auto& event : events) {
                    ImGui::PushID(&event);
                    {
                        bool changed = false;

                        changed |= this->checkbox(i18n::get_("labels.events.enabled"), event.enabled, false);
                        changed |= ImGui::Combo(i18n::get("labels.events.type").data(), reinterpret_cast<int*>(&event.type), labels::eventNames.data(), labels::eventNames.size());

                        if (event.type == labels::LabelEvent::Type::Custom) {
                            changed |= ImGui::InputText(i18n::get("labels.events.condition").data(), &event.condition);
                        }

                        {
                            int currentVisibleState = event.visible.has_value() ? (event.visible.value() ? 1 : 2) : 0;
                            changed |= ImGui::Combo(i18n::get("labels.events.visible").data(), &currentVisibleState, labels::visibleNames.data(), labels::visibleNames.size());
                            if (currentVisibleState == 0) event.visible.reset();
                            else event.visible = currentVisibleState == 1;
                        }

                        {
                            bool hasScale = event.scale.has_value();
                            if (this->checkbox(i18n::get_("labels.events.scale"), hasScale, false)) {
                                if (hasScale) event.scale = 1.0f;
                                else event.scale.reset();
                                changed = true;
                            }
                            if (hasScale) {
                                changed |= ImGui::InputFloat(i18n::get("labels.scale").data(), &event.scale.value(), 0.f, 0.f, "%.2f");
                            }
                        }

                        {
                            bool hasColor = event.color.has_value();
                            if (this->checkbox(i18n::get_("labels.events.color"), hasColor, false)) {
                                if (hasColor) event.color = {1.f, 1.f, 1.f};
                                else event.color.reset();
                                changed = true;
                            }
                            if (hasColor) {
                                changed |= ImGui::ColorEdit3(i18n::get("labels.color").data(), event.color->data(), ImGuiColorEditFlags_NoInputs);
                            }
                        }

                        {
                            bool hasOpacity = event.opacity.has_value();
                            if (this->checkbox(i18n::get_("labels.events.opacity"), hasOpacity, false)) {
                                if (hasOpacity) event.opacity = 1.0f;
                                else event.opacity.reset();
                                changed = true;
                            }
                            if (hasOpacity) {
                                changed |= ImGui::InputFloat(i18n::get("labels.opacity").data(), &event.opacity.value(), 0.f, 1.f);
                            }
                        }

                        {
                            bool hasFont = event.font.has_value();
                            if (this->checkbox(i18n::get_("labels.events.font"), hasFont, false)) {
                                if (hasFont) event.font = "bigFont.fnt";
                                else event.font.reset();
                                changed = true;
                            }
                            if (hasFont) {
                                int currentFont2 = labels::getFontIndex(event.font.value());
                                if (ImGui::Combo(i18n::get("labels.font").data(), &currentFont2, labels::fontNames.data(), labels::fontNames.size())) {
                                    event.font = labels::fontFiles[currentFont2];
                                    changed = true;
                                }
                            }
                        }

                        changed |= ImGui::InputFloat(i18n::get("labels.events.delay").data(), &event.delay, 0.f, 0.f, "%.2f");
                        changed |= ImGui::InputFloat(i18n::get("labels.events.duration").data(), &event.duration, 0.f, 0.f, "%.2f");
                        changed |= ImGui::InputFloat(i18n::get("labels.events.easing").data(), &event.easing, 0.f, 0.f, "%.2f");

                        if (this->button(i18n::get_("labels.events.delete"), false)) {
                            toDeleteIndices.push_back(&event - &events[0]);
                        }

                        if (changed) {
                            labelSettings->triggerEditCallback();
                        }
                    }
                    ImGui::Separator();
                    ImGui::PopID();
                }

                for (auto& index : toDeleteIndices) {
                    events.erase(events.begin() + index);
                }

                if (toDeleteIndices.size() > 0) {
                    labelSettings->triggerEditCallback();
                }

                if (this->button(i18n::get_("labels.events.add"), false)) {
                    events.emplace_back();
                    labelSettings->triggerEditCallback();
                }

                ImGui::Separator();
            }

            if (this->button(i18n::get_("labels.delete"), false)) {
                labelSettings->triggerDeleteCallback();
                ImGui::CloseCurrentPopup();
            }

            if (this->button(i18n::get_("labels.move-up"), false)) {
                labelSettings->triggerMoveCallback(true);
            }
            if (this->button(i18n::get_("labels.move-down"), false)) {
                labelSettings->triggerMoveCallback(false);
            }
            if (this->button(i18n::get_("labels.export"), false)) {
                labelSettings->triggerExportCallback();
            }

        }, [labelSettings, settings] {
            if (labelSettings->hasKeybind())
                handleKeybindMenu(fmt::format("label.{}", settings->id));
        }, fmt::format("label-setting-{}", settings->id)))
            labelSettings->triggerEditCallback();
    }

    bool Theme::checkbox(const std::string& label, bool& value, bool isSearchedFor, const std::function<void()>& postDraw) const {
        auto tm = ThemeManager::get();

        ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(isSearchedFor ? tm->getSearchedColor() : tm->getCheckboxForegroundColor()));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, static_cast<ImVec4>(tm->getCheckboxCheckmarkColor()));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, static_cast<ImVec4>(tm->getCheckboxBackgroundColor()));

        bool result = ImGui::Checkbox(label.c_str(), &value);
        postDraw();

        ImGui::PopStyleColor(3);
        return result;
    }

    bool Theme::checkboxWithSettings(const std::string& label, bool& value,
                                     bool isSearchedFor,
                                     const std::function<void()>& callback,
                                     const std::function<void()>& postDraw,
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
        if (ImGui::BeginPopup(popupName.c_str()/*, ImGuiWindowFlags_NoMove */)) {
            callback();
            ImGui::EndPopup();
        }

        return result;
    }

    bool Theme::button(const std::string& text, bool isSearchedFor) const {
        ImGui::PushItemWidth(-1);

        auto tm = ThemeManager::get();

        if (isSearchedFor) {
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getSearchedColor()));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, static_cast<ImVec4>(tm->getButtonForegroundColor()));
        }

        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(tm->getButtonBackgroundColor()));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(tm->getButtonHoveredBackground()));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(tm->getButtonActivatedBackground()));

        bool pressed = ImGui::Button(text.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0));

        ImGui::PopStyleColor(4);
        ImGui::PopItemWidth();

        return pressed;
    }
}


