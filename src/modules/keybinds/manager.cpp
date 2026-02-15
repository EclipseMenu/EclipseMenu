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

    static ZStringView keyToString(Keys key) {
        static std::unordered_map<Keys, std::string> const names = {
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

            { Keys::LeftShift, "Shift" }, { Keys::LeftControl, "Ctrl" }, { Keys::LeftAlt, "Alt" }, { Keys::LeftSuper, GEODE_WINDOWS("Win") GEODE_MACOS("Command") GEODE_ANDROID("Super") GEODE_IOS("Command") },
            { Keys::RightShift, "RShift" }, { Keys::RightControl, "RCtrl" }, { Keys::RightAlt, "RAlt" }, { Keys::RightSuper, GEODE_WINDOWS("RWin") GEODE_MACOS("RCommand") GEODE_ANDROID("RSuper") GEODE_IOS("RCommand") },

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

    static std::map<std::string, gui::KeybindComponent*> s_keybindComponents;
    static gui::LabelComponent* s_hintLabel;

    void updateHintLabel() {
        if (!s_hintLabel) return;
        auto& components = gui::MenuTab::find("tab.keybinds")->getComponents();
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

    Manager* Manager::get() {
        static Manager instance;
        return &instance;
    }

    inline KeybindProps getCfgKeyInternal(std::string_view id) {
        return config::get<KeybindProps>(id, Keys::None);
    }

    inline KeybindProps getCfgKey(std::string_view id) {
        return config::get<KeybindProps>(fmt::format("keybind.{}.key", id), Keys::None);
    }

    inline bool getCfgState(std::string_view id) {
        return config::get<bool>(fmt::format("keybind.{}.active", id), false);
    }

    Keybind& Manager::registerKeybindInternal(
        std::string id, std::string title, Function<void(KeyEvent)>&& callback, bool internal
    ) {
        // check if this id already exists
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) {
                return keybind;
            }
        }

        m_keybinds.emplace_back(Keys::None, std::move(id), std::move(title), std::move(callback), internal);
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
        std::string id, std::string title, Function<void(KeyEvent)>&& callback
    ) {
        return this->registerKeybindInternal(std::move(id), std::move(title), std::move(callback), false);
    }

    Keybind& Manager::addListener(std::string id, Function<void(KeyEvent)>&& callback) {
        std::string title = id;
        return this->registerKeybindInternal(std::move(id), std::move(title), std::move(callback), true);
    }

    void Manager::registerGlobalListener(Function<bool(KeyEvent)>&& callback) {
        m_globalListeners.push_back(std::move(callback));
    }

    bool Manager::unregisterKeybind(std::string const& id) {
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
                    [idStr = std::move(idStr), state, &keybind] {
                        auto tab = gui::MenuTab::find("tab.keybinds");
                        if (state) {
                            // Add the keybind to the GUI
                            auto keybindComponent = tab->addKeybind(
                                keybind.getTitle(), fmt::format("keybind.{}.key", idStr), true
                            );
                            keybindComponent->callback(
                                [tab, keybindComponent, idStr](KeybindProps key) {
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

    void Manager::registerKeyPress(KeyEvent key) {
        m_keyStates[key.props.key] = true;
        for (auto& listener : m_globalListeners) {
            if (listener(key)) {
                return;
            }
        }

        auto menuToggle = getKeybind("menu.toggle");
        if (menuToggle && key.props == menuToggle.value().get().getKey()) {
            menuToggle.value().get().execute(key);
            return;
        }

        if (shouldIgnoreInputs()) return;

        bool ignoreInPlayLayer = !utils::get<PlayLayer>() && config::get<bool>("keybind.in-game-only", false);

        for (auto& keybind : m_keybinds) {
            auto bindKey = keybind.getKey();
            bool matches = bindKey == key.props;

            if (!matches && (bindKey.key == Keys::LeftShift || bindKey.key == Keys::RightShift ||
                             bindKey.key == Keys::LeftControl || bindKey.key == Keys::RightControl ||
                             bindKey.key == Keys::LeftAlt || bindKey.key == Keys::RightAlt ||
                             bindKey.key == Keys::LeftSuper || bindKey.key == Keys::RightSuper)) {
                if (bindKey.key == key.props.key) {
                    auto expectedMods = bindKey.mods;
                    auto pressedMods = key.props.mods;
                    matches = (expectedMods & pressedMods) == expectedMods;
                }
            }

            if (matches && (keybind.isInitialized() || keybind.isInternal())) {
                if (ignoreInPlayLayer && !keybind.isInternal()) continue;
                keybind.execute(key);
            }
        }
    }

    std::optional<std::reference_wrapper<Keybind>> Manager::getKeybind(std::string_view id) {
        for (auto& keybind : m_keybinds) {
            if (keybind.getId() == id) return keybind;
        }

        return {};
    }

    void Manager::registerKeyRelease(KeyEvent key) {
        m_keyStates[key.props.key] = false;
        for (auto& listener : m_globalListeners) {
            if (listener(key)) {
                return;
            }
        }

        auto menuToggle = getKeybind("menu.toggle");
        if (menuToggle && key.props == menuToggle.value().get().getKey()) {
            return; // on release, we don't want to toggle the menu
        }

        if (shouldIgnoreInputs()) return;

        bool ignoreInPlayLayer = !utils::get<PlayLayer>() && config::get<bool>("keybind.in-game-only", false);

        for (auto& keybind : m_keybinds) {
            if (keybind.getKey() == key.props && (keybind.isInitialized() || keybind.isInternal())) {
                if (ignoreInPlayLayer && !keybind.isInternal()) continue;
                keybind.execute(key);
            }
        }
    }

    std::string keyToString(KeybindProps key) {
        StringBuffer<> buffer;

        #ifdef GEODE_IS_WINDOWS
        if (key.key != Keys::LeftControl && key.key != Keys::RightControl && key.mods & KeybindProps::Mods_Control) buffer.append("Ctrl+");
        if (key.key != Keys::LeftAlt && key.key != Keys::RightAlt && key.mods & KeybindProps::Mods_Alt) buffer.append("Alt+");
        if (key.key != Keys::LeftShift && key.key != Keys::RightShift && key.mods & KeybindProps::Mods_Shift) buffer.append("Shift+");
        if (key.key != Keys::LeftAlt && key.key != Keys::RightAlt && key.mods & KeybindProps::Mods_Super) buffer.append("Win+");
        #elif defined(GEODE_IS_MACOS)
        if (key.key != Keys::LeftControl && key.key != Keys::RightControl && key.mods & KeybindProps::Mods_Control) buffer.append("Control+");
        if (key.key != Keys::LeftAlt && key.key != Keys::RightAlt && key.mods & KeybindProps::Mods_Alt) buffer.append("Option+");
        if (key.key != Keys::LeftShift && key.key != Keys::RightShift && key.mods & KeybindProps::Mods_Shift) buffer.append("Shift+");
        if (key.key != Keys::LeftSuper && key.key != Keys::RightSuper && key.mods & KeybindProps::Mods_Super) buffer.append("Command+");
        #else
        if (key.key != Keys::LeftControl && key.key != Keys::RightControl && key.mods & KeybindProps::Mods_Control) buffer.append("Ctrl+");
        if (key.key != Keys::LeftAlt && key.key != Keys::RightAlt && key.mods & KeybindProps::Mods_Alt) buffer.append("Alt+");
        if (key.key != Keys::LeftShift && key.key != Keys::RightShift && key.mods & KeybindProps::Mods_Shift) buffer.append("Shift+");
        if (key.key != Keys::LeftSuper && key.key != Keys::RightSuper && key.mods & KeybindProps::Mods_Super) buffer.append("Super+");
        #endif

        buffer.append(keyToString(key.key));
        return buffer.str();
    }

    bool isKeyDown(Keys key) {
        auto manager = Manager::get();
        return manager->m_keyStates[key];
    }

    bool isKeyDown(KeybindProps key) {
        auto manager = Manager::get();
        bool down = manager->m_keyStates[key.key];
        if (key.mods & KeybindProps::Mods_Control) down &= (manager->m_keyStates[Keys::LeftControl] || manager->m_keyStates[Keys::RightControl]);
        if (key.mods & KeybindProps::Mods_Alt) down &= (manager->m_keyStates[Keys::LeftAlt] || manager->m_keyStates[Keys::RightAlt]);
        if (key.mods & KeybindProps::Mods_Shift) down &= (manager->m_keyStates[Keys::LeftShift] || manager->m_keyStates[Keys::RightShift]);
        if (key.mods & KeybindProps::Mods_Super) down &= (manager->m_keyStates[Keys::LeftSuper] || manager->m_keyStates[Keys::RightSuper]);
        return down;
    }

    bool isKeyPressed(Keys key) {
        auto manager = Manager::get();

        if (manager->m_keyStates[key] ^ manager->m_lastKeyStates[key])
            return manager->m_keyStates[key];

        return false;
    }

    bool isKeyPressed(KeybindProps key) {
        auto manager = Manager::get();
        bool pressed = manager->m_keyStates[key.key] && !manager->m_lastKeyStates[key.key];
        if (key.mods & KeybindProps::Mods_Control) pressed &= (manager->m_keyStates[Keys::LeftControl] || manager->m_keyStates[Keys::RightControl]);
        if (key.mods & KeybindProps::Mods_Alt) pressed &= (manager->m_keyStates[Keys::LeftAlt] || manager->m_keyStates[Keys::RightAlt]);
        if (key.mods & KeybindProps::Mods_Shift) pressed &= (manager->m_keyStates[Keys::LeftShift] || manager->m_keyStates[Keys::RightShift]);
        if (key.mods & KeybindProps::Mods_Super) pressed &= (manager->m_keyStates[Keys::LeftSuper] || manager->m_keyStates[Keys::RightSuper]);
        return pressed;
    }

    bool isKeyReleased(Keys key) {
        auto manager = Manager::get();

        if (manager->m_keyStates[key] ^ manager->m_lastKeyStates[key])
            return !manager->m_keyStates[key];

        return false;
    }

    bool isKeyReleased(KeybindProps key) {
        auto manager = Manager::get();
        bool released = !manager->m_keyStates[key.key] && manager->m_lastKeyStates[key.key];
        if (key.mods & KeybindProps::Mods_Control) released &= (manager->m_keyStates[Keys::LeftControl] || manager->m_keyStates[Keys::RightControl]);
        if (key.mods & KeybindProps::Mods_Alt) released &= (manager->m_keyStates[Keys::LeftAlt] || manager->m_keyStates[Keys::RightAlt]);
        if (key.mods & KeybindProps::Mods_Shift) released &= (manager->m_keyStates[Keys::LeftShift] || manager->m_keyStates[Keys::RightShift]);
        if (key.mods & KeybindProps::Mods_Super) released &= (manager->m_keyStates[Keys::LeftSuper] || manager->m_keyStates[Keys::RightSuper]);
        return released;
    }

    KeyboardInputData::Modifiers getCurrentModifiers() {
        KeyboardInputData::Modifiers mods = KeybindProps::Mods_None;
        if (isKeyDown(Keys::LeftControl) || isKeyDown(Keys::RightControl)) mods |= KeybindProps::Mods_Control;
        if (isKeyDown(Keys::LeftAlt) || isKeyDown(Keys::RightAlt)) mods |= KeybindProps::Mods_Alt;
        if (isKeyDown(Keys::LeftShift) || isKeyDown(Keys::RightShift)) mods |= KeybindProps::Mods_Shift;
        if (isKeyDown(Keys::LeftSuper) || isKeyDown(Keys::RightSuper)) mods |= KeybindProps::Mods_Super;
        return mods;
    }

    void Manager::setupTab() {
        // for now, we only support keybinds tab on desktop
        #ifndef GEODE_IS_MOBILE
        auto tab = gui::MenuTab::find("tab.keybinds");

        m_menuKeybindUID = tab->addKeybind("keybinds.open-menu", "menu.toggleKey")->callback(
            [](KeybindProps key) {
                if (key == Keys::MouseLeft) {
                    // Reset it back to the default keybind (LMB softlocks the menu)
                    key = Keys::Tab;
                    config::set<KeybindProps>("menu.toggleKey", Keys::Tab);
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
