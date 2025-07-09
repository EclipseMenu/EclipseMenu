#include "manager.hpp"

#include <imgui.h>
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/components/keybind.hpp>
#include <modules/gui/components/label.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/gui/imgui/imgui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/i18n/translations.hpp>

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>
#else
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#endif

using namespace geode::prelude;

namespace eclipse::keybinds {
    Keys& operator++(Keys& key) {
        key = static_cast<Keys>(static_cast<int>(key) + 1);
        return key;
    }

    Keys operator++(Keys& key, int) {
        Keys result = key;
        ++key;
        return result;
    }

    std::string const& keyToString(Keys key) {
        static const std::unordered_map<Keys, std::string> names = {
            { Keys::None, "-" },

            { Keys::A, "A" }, { Keys::B, "B" }, { Keys::C, "C" }, { Keys::D, "D" },
            { Keys::E, "E" }, { Keys::F, "F" }, { Keys::G, "G" }, { Keys::H, "H" },
            { Keys::I, "I" }, { Keys::J, "J" }, { Keys::K, "K" }, { Keys::L, "L" },
            { Keys::M, "M" }, { Keys::N, "N" }, { Keys::O, "O" }, { Keys::P, "P" },
            { Keys::Q, "Q" }, { Keys::R, "R" }, { Keys::S, "S" }, { Keys::T, "T" },
            { Keys::U, "U" }, { Keys::V, "V" }, { Keys::W, "W" }, { Keys::X, "X" },
            { Keys::Y, "Y" }, { Keys::Z, "Z" },

            { Keys::Space, "Space" }, { Keys::Apostrophe, "'" }, { Keys::Comma, "," },
            { Keys::Minus, "-" }, { Keys::Period, "." }, { Keys::Slash, "/" }, { Keys::Semicolon, ";" },
            { Keys::Equal, "=" }, { Keys::LeftBracket, "[" }, { Keys::Backslash, "\\" },
            { Keys::RightBracket, "]" }, { Keys::GraveAccent, "`" }, { Keys::World1, "World1" },
            { Keys::World2, "World2" },

            { Keys::Num0, "0" }, { Keys::Num1, "1" }, { Keys::Num2, "2" }, { Keys::Num3, "3" },
            { Keys::Num4, "4" }, { Keys::Num5, "5" }, { Keys::Num6, "6" }, { Keys::Num7, "7" },
            { Keys::Num8, "8" }, { Keys::Num9, "9" },

            { Keys::F1, "F1" }, { Keys::F2, "F2" }, { Keys::F3, "F3" }, { Keys::F4, "F4" },
            { Keys::F5, "F5" }, { Keys::F6, "F6" }, { Keys::F7, "F7" }, { Keys::F8, "F8" },
            { Keys::F9, "F9" }, { Keys::F10, "F10" }, { Keys::F11, "F11" }, { Keys::F12, "F12" },
            { Keys::F13, "F13" }, { Keys::F14, "F14" }, { Keys::F15, "F15" }, { Keys::F16, "F16" },
            { Keys::F17, "F17" }, { Keys::F18, "F18" }, { Keys::F19, "F19" }, { Keys::F20, "F20" },
            { Keys::F21, "F21" }, { Keys::F22, "F22" }, { Keys::F23, "F23" }, { Keys::F24, "F24" },
            { Keys::F25, "F25" },

            { Keys::NumPad0, "Num 0" }, { Keys::NumPad1, "Num 1" }, { Keys::NumPad2, "Num 2" },
            { Keys::NumPad3, "Num 3" }, { Keys::NumPad4, "Num 4" }, { Keys::NumPad5, "Num 5" },
            { Keys::NumPad6, "Num 6" }, { Keys::NumPad7, "Num 7" }, { Keys::NumPad8, "Num 8" },
            { Keys::NumPad9, "Num 9" }, { Keys::NumPadDecimal, "Num ." }, { Keys::NumPadDivide, "Num /" },
            { Keys::NumPadMultiply, "Num *" }, { Keys::NumPadSubtract, "Num -" }, { Keys::NumPadAdd, "Num +" },
            { Keys::NumPadEnter, "Num Enter" }, { Keys::NumPadEqual, "Num =" },

            { Keys::Menu, "Menu" }, { Keys::Escape, "Escape" }, { Keys::Enter, "Enter" }, { Keys::Tab, "Tab" },
            { Keys::Backspace, "Backspace" }, { Keys::Insert, "Insert" }, { Keys::Delete, "Delete" },
            { Keys::Home, "Home" }, { Keys::End, "End" }, { Keys::PageUp, "Page Up" }, { Keys::PageDown, "Page Down" },
            { Keys::CapsLock, "Caps Lock" }, { Keys::ScrollLock, "Scroll Lock" }, { Keys::NumLock, "Num Lock" },
            { Keys::PrintScreen, "Print Screen" }, { Keys::Pause, "Pause" },

            { Keys::Up, "Up" }, { Keys::Down, "Down" }, { Keys::Left, "Left" }, { Keys::Right, "Right" },

            { Keys::LeftShift, "Shift" }, { Keys::LeftControl, "Ctrl" }, { Keys::LeftAlt, "Alt" }, { Keys::LeftSuper, "Super" },
            { Keys::RightShift, "RShift" }, { Keys::RightControl, "RCtrl" }, { Keys::RightAlt, "RAlt" }, { Keys::RightSuper, "RSuper" },

            { Keys::MouseLeft, "LMB" }, { Keys::MouseRight, "RMB" }, { Keys::MouseMiddle, "MMB" },
            { Keys::MouseButton4, "Mouse 4" }, { Keys::MouseButton5, "Mouse 5" },

            { Keys::MenuKey, "Menu Key" }, { Keys::LastKey, "Last Key" },
            { Keys::Unknown, "Unknown" }
        };
        if (auto it = names.find(key); it != names.end()) {
            return it->second;
        }
        return names.at(Keys::Unknown);
    }

    static std::map<std::string, std::shared_ptr<gui::KeybindComponent>> s_keybindComponents;
    static std::shared_ptr<gui::LabelComponent> s_hintLabel;

    void updateHintLabel() {
        if (!s_hintLabel) return;
        auto components = gui::MenuTab::find("tab.keybinds")->getComponents();
        s_hintLabel->setText(
            components.size() <= 3 ? i18n::get_("keybinds.hint") : ""
        );

        // if in cocos ui, refresh the page
        if (auto cocos = gui::cocos::CocosRenderer::get()) {
            if (cocos->getSelectedTab() == "tab.keybinds") {
                cocos->refreshPage();
            }
        }
    }

    std::shared_ptr<Manager> Manager::get() {
        static auto instance = std::make_shared<Manager>();
        return instance;
    }

    inline Keys getCfgKeyInternal(std::string_view id) {
        return config::get<Keys>(id, Keys::None);
    }

    inline Keys getCfgKey(std::string_view id) {
        return config::get<Keys>(fmt::format("keybind.{}.key", id), Keys::None);
    }

    inline bool getCfgState(std::string_view id) {
        return config::get<bool>(fmt::format("keybind.{}.active", id), false);
    }

    Keybind& Manager::registerKeybindInternal(
        const std::string& id, const std::string& title, const std::function<void(bool)>& callback, bool internal
    ) {
        // check if this id already exists
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) {
                return keybind;
            }
        }

        m_keybinds.emplace_back(Keys::None, id, title, callback, internal);
        auto& keybind = m_keybinds.back();
        if (internal) {
            keybind.setKey(getCfgKeyInternal(keybind.getId()));
            this->setKeybindState(id, true);
        } else if (m_initialized) {
            keybind.setKey(getCfgKey(keybind.getId()));
            this->setKeybindState(keybind.getId(), getCfgState(keybind.getId()));
        }

        return keybind;
    }

    Keybind& Manager::registerKeybind(
        const std::string& id, const std::string& title, const std::function<void(bool)>& callback
    ) {
        return this->registerKeybindInternal(id, title, callback, false);
    }

    Keybind& Manager::addListener(const std::string& id, const std::function<void(bool)>& callback) {
        return this->registerKeybindInternal(id, id, callback, true);
    }

    bool Manager::unregisterKeybind(const std::string& id) {
        for (auto it = m_keybinds.begin(); it != m_keybinds.end(); ++it) {
            if (it->getId() == id) {
                // apply disabled state to process the GUI
                this->setKeybindState(id, false);

                // then delete the keybind from the config
                m_keybinds.erase(it);
                return true;
            }
        }
        return false;
    }

    void Manager::init() {
        setupTab();

        for (auto& keybind : m_keybinds) {
            if (!keybind.isInternal()) {
                keybind.setKey(getCfgKey(keybind.getId()));
                this->setKeybindState(keybind.getId(), getCfgState(keybind.getId()));
            }
        }

        m_initialized = true;
    }

    void Manager::update() {
        for (auto& [key, state] : m_keyStates) {
            m_lastKeyStates.insert_or_assign(key, state);
        }
    }

    void Manager::setKeybindState(std::string_view id, bool state) {
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) {
                keybind.setInitialized(state);
                if (!state) {
                    config::erase(fmt::format("keybind.{}.active", id));
                } else {
                    config::set(fmt::format("keybind.{}.active", id), state);
                }

                // do not update GUI on mobile
                #ifndef GEODE_IS_MOBILE
                if (keybind.isInternal()) return;

                auto idStr = std::string(id);
                gui::Engine::queueAfterDrawing(
                    [idStr, state, keybind] {
                        auto tab = gui::MenuTab::find("tab.keybinds");
                        if (state) {
                            // Add the keybind to the GUI
                            auto keybindComponent = tab->addKeybind(
                                keybind.getTitle(), fmt::format("keybind.{}.key", idStr), true
                            );
                            keybindComponent->callback(
                                [tab, keybindComponent, idStr](Keys key) {
                                    auto keybind = Manager::get()->getKeybind(idStr);

                                    if (!keybind.has_value()) return;

                                    auto& keybindRef = keybind->get();

                                    if (key == Keys::None) {
                                        config::erase(fmt::format("keybind.{}.active", idStr));
                                        keybindRef.setInitialized(false);
                                        gui::Engine::queueAfterDrawing(
                                            [tab, keybindComponent] {
                                                tab->removeComponent(keybindComponent);
                                                updateHintLabel();
                                            }
                                        );
                                    }

                                    keybindRef.setKey(key);
                                }
                            );

                            s_keybindComponents[idStr] = keybindComponent;
                        } else {
                            // Reset the keybind to None
                            config::erase(fmt::format("keybind.{}.key", idStr));

                            // Remove the keybind from the GUI
                            if (auto keybindComponent = s_keybindComponents[idStr]; keybindComponent) {
                                tab->removeComponent(keybindComponent);
                            }
                        }

                        updateHintLabel();
                    }
                );

                return;
                #endif
            }
        }

        geode::log::warn("Keybind with ID '{}' not found", id);
    }

    inline bool shouldIgnoreInputs() {
        // Ignore if imgui is capturing inputs or if the keyboard is being used
        if (gui::imgui::ImGuiRenderer::get() && ImGui::GetIO().WantTextInput)
            return true;

        // Ignore if the keyboard is being used
        if (utils::get<CCIMEDispatcher>()->hasDelegate())
            return true;

        return false;
    }

    void Manager::registerKeyPress(Keys key) {
        m_keyStates[key] = true;

        auto menuToggle = getKeybind("menu.toggle");
        if (menuToggle && key == menuToggle.value().get().getKey()) {
            menuToggle.value().get().push();
            return;
        }

        if (shouldIgnoreInputs()) return;

        bool ignoreInPlayLayer = !utils::get<PlayLayer>() && config::get<bool>("keybind.in-game-only", false);

        for (auto& keybind : m_keybinds) {
            if (keybind.getKey() == key && (keybind.isInitialized() || keybind.isInternal())) {
                if (ignoreInPlayLayer && !keybind.isInternal()) continue;
                keybind.push();
            }
        }
    }

    std::optional<std::reference_wrapper<Keybind>> Manager::getKeybind(std::string_view id) {
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) return keybind;
        }

        return {};
    }

    void Manager::registerKeyRelease(Keys key) {
        m_keyStates[key] = false;

        auto menuToggle = getKeybind("menu.toggle");
        if (menuToggle && key == menuToggle.value().get().getKey()) {
            return; // on release, we don't want to toggle the menu
        }

        if (shouldIgnoreInputs()) return;

        bool ignoreInPlayLayer = !utils::get<PlayLayer>() && config::get<bool>("keybind.in-game-only", false);

        for (auto& keybind : m_keybinds) {
            if (keybind.getKey() == key && (keybind.isInitialized() || keybind.isInternal())) {
                if (ignoreInPlayLayer && !keybind.isInternal()) continue;
                keybind.release();
            }
        }
    }

    bool isKeyDown(Keys key) {
        auto manager = Manager::get();
        return manager->m_keyStates[key];
    }

    bool isKeyPressed(Keys key) {
        auto manager = Manager::get();

        if (manager->m_keyStates[key] ^ manager->m_lastKeyStates[key])
            return manager->m_keyStates[key];

        return false;
    }

    bool isKeyReleased(Keys key) {
        auto manager = Manager::get();

        if (manager->m_keyStates[key] ^ manager->m_lastKeyStates[key])
            return !manager->m_keyStates[key];

        return false;
    }

    void Manager::setupTab() {
        // for now, we only support keybinds tab on desktop
        #ifndef GEODE_IS_MOBILE
        auto tab = gui::MenuTab::find("tab.keybinds");

        m_menuKeybindUID = tab->addKeybind("keybinds.open-menu", "menu.toggleKey")->callback(
            [](Keys key) {
                if (key == Keys::MouseLeft) {
                    // Reset it back to the default keybind (LMB softlocks the menu)
                    key = Keys::Tab;
                    config::set("menu.toggleKey", Keys::Tab);
                }

                if (auto keybind = Manager::get()->getKeybind("menu.toggle"); keybind.has_value())
                    keybind->get().setKey(key);
            }
        )->setDefaultKey(Keys::Tab)->getUID();

        tab->addToggle("keybinds.in-game-only")
           ->setDescription();

        s_hintLabel = tab->addLabel("");
        updateHintLabel();
        #endif
    }
}
