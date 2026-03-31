#include <asp/fs/fs.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

constexpr auto CHATBOT_CONFIG_URL = "https://eclipse.menu/chatbot.json";
constexpr auto CHATBOT_UPDATE_INTERVAL = 60 * 60 * 4;

$on_mod(Loaded) {
    auto lastCheck = Mod::get()->getSavedValue<uint64_t>("lastChatbotDownload", 0);

    auto now = asp::SystemTime::now();
    auto lastCheckTime = asp::SystemTime::fromUnix(lastCheck);
    if (now - lastCheckTime < asp::Duration::fromSecs(CHATBOT_UPDATE_INTERVAL)) {
        if (asp::fs::exists(Mod::get()->getSaveDir() / "chatbot.json"))
            return;
    }

    async::spawn(
        web::WebRequest().get(CHATBOT_CONFIG_URL),
        [](web::WebResponse response) {
            if (!response.ok()) {
                log::error("Failed to fetch chatbot config: {}", response.errorMessage());
                return;
            }

            auto res = response.into(Mod::get()->getSaveDir() / "chatbot.json");
            if (!res) {
                log::error("Failed to save chatbot config: {}", res.unwrapErr());
                return;
            }

            Mod::get()->setSavedValue("lastChatbotDownload", asp::SystemTime::now().timeSinceEpoch().seconds());
        }
    );
}