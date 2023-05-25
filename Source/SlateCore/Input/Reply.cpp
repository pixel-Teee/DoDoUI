#include <PreCompileHeader.h>

#include "Reply.h"

namespace DoDo {
    FReply& FReply::set_user_focus(std::shared_ptr<SWidget> give_me_focus, EFocusCause reason_focus_is_changing, bool b_in_all_users)
    {
        this->m_b_set_user_focus = true;
        this->m_focus_recipient = give_me_focus;

        return Me();
    }
    FReply& FReply::clear_user_focus(EFocusCause reason_focus_is_changing, bool b_in_all_users)
    {
        this->m_b_set_user_focus = false;
        this->m_focus_recipient.reset();

        return Me();
    }
}