#ifndef MOD_WEEKENDBONUS_H
#define MOD_WEEKENDBONUS_H

#include "ScriptMgr.h"
#include <time.h>

enum Day
{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

enum BonusTypes
{
    BONUS_NONE = 0,
    BONUS_WEEKEND,
    BONUS_EVENING,
    BONUS_HOLIDAY
};

enum BonusMultipliers
{
    BM_WEEKEND = 0,
    BM_EVENING,
    BM_HOLIDAY,
    BM_LAST
};

enum MessageTypes
{
    MSG_BONUS_START = 0,
    MSG_BONUS_ACTIVE,
    MSG_BONUS_END,
    MSG_LAST
};

class WeekendBonus : public PlayerScript, WorldScript
{
public:
    WeekendBonus();

    // PlayerScript
    void OnPlayerLogin(Player* /*player*/) override;

    // WorldScript
    void OnAfterConfigLoad(bool /*reload*/) override;
    void OnStartup() override;
    void OnUpdate(uint32 /*diff*/) override;

private:
    void LoadDefaultValues();
    void SetRates(bool /*active*/);
    bool HasActiveMultipliers();
    void UpdateLocalTime();
    BonusTypes GetCurrentBonusType();
    void DoBonusUpdateCheck(uint32 diff);
    bool IsTodayHoliday();
    std::string CheckForNamedHoliday(const std::string& holidayName);
    std::string GetMessageText(BonusTypes bonusType, MessageTypes messageType, bool needColorCodes, std::string colorCode = "ff4CFF00");
    std::string GetFormattedTimestamp();

    bool Triggered;
    BonusTypes m_BonusType;
    BonusMultipliers m_BonusMultiplier;
    time_t LocalTime;
    struct tm tm_LocalTime;
    int int_LocalTime;
    int m_EveningStart;
    int m_EveningEnd;
    Milliseconds CheckFrequency;
    Milliseconds CheckTime;
    Milliseconds AnnouncementFrequency;
    Milliseconds AnnouncementTime;
    std::string m_NamedHoliday;

    // weekend multipliers
    float m_ExperienceMultiplier[BonusMultipliers::BM_LAST];
    float m_MoneyMultiplier[BonusMultipliers::BM_LAST];
    uint32 m_ProfessionsMultiplier[BonusMultipliers::BM_LAST];
    float m_ReputationMultiplier[BonusMultipliers::BM_LAST];
    uint32 m_ProficienciesMultiplier[BonusMultipliers::BM_LAST];
    float m_HonorMultiplier[BonusMultipliers::BM_LAST];

    // other optional multipliers
    bool m_EveningEnabled;
    bool m_HolidayEnabled;

    std::vector<std::pair<int, int>> m_HolidayDates; // month, day

    // default multipliers
    float m_DefaultExperienceMultiplier[6];
    float m_DefaultBattlegroundExperienceMultiplier[6];
    uint32 m_DefaultProfessionsMultiplier[2];
    uint32 m_DefaultProficienciesMultiplier[2];
    float m_DefaultMoneyMultiplier;
    float m_DefaultReputationMultiplier;
    float m_DefaultHonorMultiplier;
};

#endif
