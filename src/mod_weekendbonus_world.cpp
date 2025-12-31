#include "mod_weekendbonus.h"

#include "WorldSessionMgr.h"

#if WEEKENDBONUS_DEBUG
#include <format>
#endif

// 18:00:00 to 22:00:00 (6 PM to 10 PM) is considered evening
#define EVENING_START 180000
#define EVENING_END 220000

void WeekendBonus::OnStartup()
{
    LOG_INFO("weekendbonus", "> WeekendBonus Module Started");
    m_BonusMultiplier = BM_WEEKEND;
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

#if WEEKENDBONUS_DEBUG
    LOG_INFO("weekendbonus", "> CHK: {}, EE: {}, HE: {}, TIME: {}, TRIGGERED: {}, BONUS: {}, HOLIDAY: {}", 
        CheckTime, m_EveningEnabled, m_HolidayEnabled, int_LocalTime, Triggered, (int)m_BonusType, m_NamedHoliday);
#endif

    if (Triggered)
    {
        if (bonus == BONUS_NONE && m_BonusType != BONUS_NONE)
        {
            // the bonus period has ended
            SetRates(false);
            if (m_BonusType == BONUS_WEEKEND)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "|cffadb5bdThe weekend bonus is no longer active.|r");
            }
            else if (m_BonusType == BONUS_EVENING)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "|cffadb5bdThe evening bonus is no longer active.|r");
            }
            else if (m_BonusType == BONUS_HOLIDAY)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "|cffadb5bdThe holiday bonus is no longer active.|r");
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
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The |cff4CFF00weekend bonus|r is active, granting you bonuses!");
                }
                else if (m_BonusType == BONUS_EVENING)
                {
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The |cff4CFF00evening bonus|r is active, granting you bonuses!");
                }
                else if (m_BonusType == BONUS_HOLIDAY)
                {
                    sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The |cff4CFF00holiday bonus|r is active, granting you bonuses!");
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
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The |cff4CFF00weekend bonus|r is now active, granting you bonuses!");
            }
            else if (bonus == BONUS_EVENING)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The |cff4CFF00evening bonus|r is now active, granting you bonuses!");
            }
            else if (bonus == BONUS_HOLIDAY)
            {
                sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING, "The |cff4CFF00holiday bonus|r is now active, granting you bonuses!");
            }
        }
    }    
}

bool WeekendBonus::IsTodayHoliday()
{
    if (m_HolidayDates.empty()) return false;
    for (const auto& date : m_HolidayDates)
    {
#if WEEKENDBONUS_DEBUG
        LOG_INFO("weekendbonus", "> HM: {}, HD: {}, LM: {}, LD: {}", 
            date.first, date.second, tm_LocalTime.tm_mon + 1, tm_LocalTime.tm_mday);
#endif
        if ((tm_LocalTime.tm_mon + 1) == date.first && tm_LocalTime.tm_mday == date.second)
        {
            return true;
        }
    }

    return false;
}

bool WeekendBonus::HasActiveMultipliers()
{
    int idx = static_cast<int>(m_BonusMultiplier);
    if (m_ExperienceMultiplier[idx] > 1 || m_MoneyMultiplier[idx] > 1 || m_ProfessionsMultiplier[idx] > 1 || m_ReputationMultiplier[idx] > 1 ||
            m_ProficienciesMultiplier[idx] > 1 || m_HonorMultiplier[idx] > 1)
    {
        return true;
    }

    return false;
}

void WeekendBonus::UpdateLocalTime()
{
    LocalTime = time(NULL);
    #if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_LocalTime, &LocalTime);
    #else
        localtime_r(&LocalTime, &tm_LocalTime);
    #endif    
    // returns time as integer, i.e. 23:19:35 would be 231935
    int_LocalTime = (tm_LocalTime.tm_hour * 10000) + (tm_LocalTime.tm_min * 100) + tm_LocalTime.tm_sec;
}

BonusTypes WeekendBonus::GetCurrentBonusType()
{
    if (m_HolidayEnabled && IsTodayHoliday())
    {
        m_BonusMultiplier = BM_HOLIDAY;
        return BONUS_HOLIDAY;
    }
    else if ((tm_LocalTime.tm_wday == Day::FRIDAY && tm_LocalTime.tm_hour >= 18) ||
            tm_LocalTime.tm_wday == Day::SATURDAY || tm_LocalTime.tm_wday == Day::SUNDAY)
    {
        m_BonusMultiplier = BM_WEEKEND;
        return BONUS_WEEKEND;
    }
    else if (m_EveningEnabled && (int_LocalTime >= EVENING_START  && int_LocalTime < EVENING_END))
    {
        m_BonusMultiplier = BM_EVENING;
        return BONUS_EVENING;
    }

    // no bonus active, use weekend as default multiplier
    m_BonusMultiplier = BM_WEEKEND;
    return BONUS_NONE;
}
