#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CommentCell.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(ShowSpamComments) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.showspamcomments")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Show Spam Comments"; }
    };

    REGISTER_HACK(ShowSpamComments)

    class $modify(ShowSpamCommentsCommentCellHook, CommentCell) {
        ADD_HOOKS_DELEGATE("bypass.showspamcomments")

        void loadFromComment(GJComment* comment) {
            CommentCell::loadFromComment(comment);
            this->onUnhide(nullptr);
        }
    };
}