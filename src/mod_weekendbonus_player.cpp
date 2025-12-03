#include "Chat.h"

#include "mod_weekendbonus.h"

void WeekendBonus::OnPlayerLogin(Player* player)
{
    BonusTypes bonus = GetCurrentBonusType();

    if (bonus == BONUS_WEEKEND)
    {
        ChatHandler(player->GetSession()).SendSysMessage("The weekend bonus is active, granting you bonuses!");
    }
    else if (bonus == BONUS_EVENING)
    {
        ChatHandler(player->GetSession()).SendSysMessage("The evening bonus is active, granting you bonuses!");
    }    
    else if (bonus == BONUS_HOLIDAY)
    {
        ChatHandler(player->GetSession()).SendSysMessage("The holiday bonus is active, granting you bonuses!");
    }    
}
