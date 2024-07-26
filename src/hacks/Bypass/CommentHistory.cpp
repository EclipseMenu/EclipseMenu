#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/ProfilePage.hpp>

namespace eclipse::hacks::Bypass {

    class CommentHistory : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Comment History Bypass", "bypass.commenthistory")
                ->handleKeybinds()
                ->setDescription("Allows you to see any user's comment history");
        }

        [[nodiscard]] const char* getId() const override { return "Comment History Bypass"; }
    };

    REGISTER_HACK(CommentHistory)

    class $modify(ProfilePage) {
        void loadPageFromUserInfo(GJUserScore* score) {
            auto originalCommentHistory = score->m_commentHistoryStatus;

            if (config::get<bool>("bypass.commenthistory", false))
                score->m_commentHistoryStatus = 0;

            ProfilePage::loadPageFromUserInfo(score);

            score->m_commentHistoryStatus = originalCommentHistory;
        }
    };

}
