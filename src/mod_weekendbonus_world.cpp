#include "mod_weekendbonus.h"

#include "WorldSessionMgr.h"

#if WEEKENDBOUS_DEBUG
#include <format>
#endif

// 18:00:00 to 22:00:00 (6 PM to 10 PM) is considered evening
#define EVENING_START 180000
#define EVENING_END 220000

void WeekendBonus::OnStartup()
{
    if (!HasActiveMultipliers())
    {
        return;
    }

    Triggered = false;
    DoBonusUpdateCheck(0);
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
        DoBonusUpdateCheck(diff);
        CheckTime = 0s;
    }
}

void WeekendBonus::DoBonusUpdateCheck(uint32 diff)
{
    UpdateLocalTime();
    BonusTypes bonus = GetCurrentBonusType();

#if WEEKENDBOUS_DEBUG
    //sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, std::format("HOUR: {}, MIN: {}, SEC: {}",
        //tm_LocalTime->tm_hour, tm_LocalTime->tm_min, tm_LocalTime->tm_sec));

    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, std::format("CHK: {}, CONFIG: {}, TIME: {}, TRIGGERED: {}, BONUS: {}",
        CheckTime, m_EveningEnabled, int_LocalTime, Triggered, (int)m_BonusType));
#endif

    if (Triggered)
    {
        if (bonus == BONUS_NONE && m_BonusType != BONUS_NONE)
        {
            // the bonus period has ended
            SetRates(false);
            if (m_BonusType == BONUS_WEEKEND)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The weekend bonus is no longer active.");
            }
            else if (m_BonusType == BONUS_EVENING)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The evening bonus is no longer active.");
            }
            m_BonusType = BONUS_NONE;
        }
        else
        {
            // still inside the bonus period, see if it is time to announce
            AnnouncementTime += Milliseconds(diff);
            if (AnnouncementTime > AnnouncementFrequency)
            {
                if (m_BonusType == BONUS_WEEKEND)
                {
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The weekend bonus is active, granting you bonuses!");
                }
                else if (m_BonusType == BONUS_EVENING)
                {
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The evening bonus is active, granting you bonuses!");
                }
                AnnouncementTime = 0s;
            }
        }
    }
    else
    {
        if (bonus != BONUS_NONE && m_BonusType == BONUS_NONE)
        {
            // a bonus period has started
            m_BonusType = bonus;
            SetRates(true);
            if (bonus == BONUS_WEEKEND)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The weekend bonus is now active, granting you bonuses!");
            }
            else if (bonus == BONUS_EVENING)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The evening bonus is now active, granting you bonuses!");
            }
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

BonusTypes WeekendBonus::GetCurrentBonusType()
{
    if ((tm_LocalTime->tm_wday == Day::FRIDAY && tm_LocalTime->tm_hour >= 18) ||
            tm_LocalTime->tm_wday == Day::SATURDAY || tm_LocalTime->tm_wday == Day::SUNDAY)
    {
        return BONUS_WEEKEND;
    }
    else if (m_EveningEnabled && (int_LocalTime >= EVENING_START  && int_LocalTime < EVENING_END))
    {
        return BONUS_EVENING;
    }

    return BONUS_NONE;
}
