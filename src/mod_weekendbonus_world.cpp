#include "mod_weekendbonus.h"

#include "WorldSessionMgr.h"
#include <format>

// 18:00:00 to 23:59:59 is considered nights
#define NIGHTTIME_START 180000   // 120000
#define NIGHTTIME_END 235959     // 125959

void WeekendBonus::OnStartup()
{
    if (!HasActiveMultipliers())
    {
        return;
    }

    Triggered = false;
    UpdateLocalTime();

    if ((tm_LocalTime->tm_wday == Day::FRIDAY && tm_LocalTime->tm_hour >= 18) ||
            tm_LocalTime->tm_wday == Day::SATURDAY || tm_LocalTime->tm_wday == Day::SUNDAY)
    {
        BonusType = BONUS_WEEKEND;
        SetRates(true);
    }
    else if (IsNightTime())
    {
        BonusType = BONUS_NIGHTTIME;
        SetRates(true);
    }
}

void WeekendBonus::OnUpdate(uint32 diff)
{
    if (!HasActiveMultipliers())
    {
        return;
    }

    CheckTime += Milliseconds(diff);
    if (CheckTime > CheckFrequency)
    {
        UpdateLocalTime();

        //sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, std::format("HOUR: {}, MIN: {}, SEC: {}",
            //tm_LocalTime->tm_hour, tm_LocalTime->tm_min, tm_LocalTime->tm_sec));

        //sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, std::format("CHK: {}, CONFIG: {}, TIME: {}, TRIGGERED: {}, BONUS: {}",
            //CheckTime, NightsEnabled, int_LocalTime, Triggered, (int)BonusType));

        if ((tm_LocalTime->tm_wday == Day::FRIDAY && tm_LocalTime->tm_hour >= 18) && !Triggered)
        {
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The weekend bonus is now active, granting you bonuses!");
            BonusType = BONUS_WEEKEND;
            SetRates(true);
        }
        else if (IsNightTime() && !Triggered)
        {
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The night time bonus is now active, granting you bonuses!");
            BonusType = BONUS_NIGHTTIME;
            SetRates(true);
        }
        else if ((int_LocalTime < NIGHTTIME_START || int_LocalTime > NIGHTTIME_END) && Triggered)
        {
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The night time bonus is no longer active.");
            BonusType = BONUS_NONE;
            SetRates(false);
        }
        else if (tm_LocalTime->tm_wday == Day::MONDAY && Triggered)
        {
            sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The weekend bonus is no longer active.");
            BonusType = BONUS_NONE;
            SetRates(false);
        }

        CheckTime = 0s;
    }

    if (Triggered)
    {
        AnnouncementTime += Milliseconds(diff);
        if (AnnouncementTime > AnnouncementFrequency)
        {
            // TODO: fix this to show message if it is night time
            if (BonusType == BONUS_WEEKEND)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The weekend bonus is active, granting you bonuses!");
            }
            else if (BonusType == BONUS_NIGHTTIME)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The night time bonus is active, granting you bonuses!");
            }
            AnnouncementTime = 0s;
        }
    }
}

bool WeekendBonus::HasActiveMultipliers()
{
    if (ExperienceMultiplier > 1 || MoneyMultiplier > 1 || ProfessionsMultiplier > 1 || ReputationMultiplier > 1 ||
            ProficienciesMultiplier > 1 || HonorMultiplier > 1)
    {
        return true;
    }

    return false;
}

void WeekendBonus::UpdateLocalTime()
{
    LocalTime = time(NULL);
    tm_LocalTime = localtime(&LocalTime);
    // returns time as integer, i.e. 23:19:35 would be 231935
    int_LocalTime = (tm_LocalTime->tm_hour * 10000) + (tm_LocalTime->tm_min * 100) + tm_LocalTime->tm_sec;
}

bool WeekendBonus::IsNightTime()
{
    if (!NightsEnabled) return false;

    if (int_LocalTime >= NIGHTTIME_START  && int_LocalTime <= NIGHTTIME_END)
    {
        //sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "Nighttime period!");
        return true;
    }

    return false;
}
