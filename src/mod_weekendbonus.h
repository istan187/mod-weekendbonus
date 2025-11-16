#ifndef MOD_WEEKENDBONUS_H
#define MOD_WEEKENDBONUS_H

#include "ScriptMgr.h"

#define WEEKENDBOUS_DEBUG 0

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
    BONUS_EVENING
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

    bool Triggered;
    BonusTypes m_BonusType;
    time_t LocalTime;
    tm* tm_LocalTime;
    int int_LocalTime;
    Milliseconds CheckFrequency;
    Milliseconds CheckTime;
    Milliseconds AnnouncementFrequency;
    Milliseconds AnnouncementTime;

    float ExperienceMultiplier;
    float MoneyMultiplier;
    uint32 ProfessionsMultiplier;
    float ReputationMultiplier;
    uint32 ProficienciesMultiplier;
    float HonorMultiplier;
    bool m_EveningEnabled;

    float DefaultExperienceMultiplier[6];
    float DefaultBattlegroundExperienceMultiplier[6];
    uint32 DefaultProfessionsMultiplier[2];
    uint32 DefaultProficienciesMultiplier[2];
    float DefaultMoneyMultiplier;
    float DefaultReputationMultiplier;
    float DefaultHonorMultiplier;
};

#endif
