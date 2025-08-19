#pragma once

#ifdef GEODE_IS_WINDOWS
#ifdef VIPER___EXTRACRASHDATA____EXPORTING
#define VIPER___EXTRACRASHDATA____DLL __declspec(dllexport)
#else
#define VIPER___EXTRACRASHDATA____DLL __declspec(dllimport)
#endif
#else
#define VIPER___EXTRACRASHDATA____DLL __attribute__((visibility("default")))
#endif

#include <Geode/loader/Event.hpp>

class VIPER___EXTRACRASHDATA____DLL ExtraCrashData {
public:
    class ModData {
        public:
            virtual std::string OnCrash() {
                return "This is the default string";
            };
    };
    class ModDataLambda : public ModData {
        public:
            std::function<std::string()> call;
            virtual std::string OnCrash() {
                return call();
            };
    };
    struct Message {
        enum Type {
            BeforeGeodeInfo,
            AfterGeodeInfo,
            BeforeMod,
            WithMod,
            AfterMod
        };
    };
    template <typename F>
    static void waitforMod(F&& callback) {
        if (geode::Loader::get()->getLoadedMod("cosmella.extra_crash_data") != nullptr) {
            callback();
        } else {
            auto mod = geode::Loader::get()->getInstalledMod("cosmella.extra_crash_data");
            if (!mod) return;
            new geode::EventListener(
                [callback = std::forward<F>(callback)](geode::ModStateEvent*) {
                    callback();
                },
                geode::ModStateFilter(mod, geode::ModEventType::Loaded)
            );
        }
    }
    struct CrashlogEvent final : geode::Event {
        CrashlogEvent(ModData* callback, Message::Type Type)
            : m_self(callback), m_type(Type) {};
        
        CrashlogEvent(std::function<std::string()> callback, Message::Type Type) {
                        m_type = Type;
                        ModDataLambda* lamb = new ModDataLambda();
                        lamb->call = std::move(callback);
                        m_self = lamb;
            }
        ModData* m_self;
        Message::Type m_type;
    };
};