#include "Chat.h"

#include "mod_weekendbonus.h"

void WeekendBonus::OnPlayerLogin(Player* player)
{
    if ((tm_LocalTime->tm_wday == Day::FRIDAY && tm_LocalTime->tm_hour >= 18) ||
            tm_LocalTime->tm_wday == Day::SATURDAY || tm_LocalTime->tm_wday == Day::SUNDAY)
    {
        ChatHandler(player->GetSession()).SendSysMessage("The weekend bonus is active, granting you bonuses!");
    }
    else if (IsNightTime())
    {
        ChatHandler(player->GetSession()).SendSysMessage("The night time bonus is active, granting you bonuses!");
    }

}
