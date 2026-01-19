#include "Chat.h"

#include "mod_weekendbonus.h"

void WeekendBonus::OnPlayerLogin(Player* player)
{
    BonusTypes bonus = GetCurrentBonusType();
    if (bonus != BONUS_NONE)
    {
        ChatHandler(player->GetSession()).SendSysMessage(GetMessageText(bonus, MSG_BONUS_ACTIVE, true));
    }
}
