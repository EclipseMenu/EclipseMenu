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
                ->setDescription("Allows you to see any user's comment history. (Created by Uproxide)");
        }

        [[nodiscard]] const char* getId() const override { return "Comment History Bypass"; }
    };

    REGISTER_HACK(CommentHistory)

    class $modify(CommentHistoryPPHook, ProfilePage) {
        ADD_HOOKS_DELEGATE("bypass.commenthistory")

        void loadPageFromUserInfo(GJUserScore* score) {
            int originalCommentHistory = score->m_commentHistoryStatus;
            score->m_commentHistoryStatus = 0;
            ProfilePage::loadPageFromUserInfo(score);
            score->m_commentHistoryStatus = originalCommentHistory;
        }
    };

}
