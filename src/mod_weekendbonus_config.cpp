#include "mod_weekendbonus.h"

#include "Config.h"

void WeekendBonus::OnAfterConfigLoad(bool reload)
{
    if (!reload)
    {
        LoadDefaultValues();
    }

    ExperienceMultiplier = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Experience", 2.0f);
    MoneyMultiplier = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Money", 2.0f);
    ProfessionsMultiplier = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Professions", 2);
    ReputationMultiplier = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Reputation", 2.0f);
    ProficienciesMultiplier = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Proficiencies", 2);
    HonorMultiplier = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Honor", 2.0f);
    m_EveningEnabled = sConfigMgr->GetOption<bool>("WeekendBonus.Evening.Bonuses.Enabled", 0);

    if (reload)
    {
        m_BonusType = GetCurrentBonusType();
        SetRates(m_BonusType != BONUS_NONE);
    }
}

void WeekendBonus::LoadDefaultValues()
{
    DefaultExperienceMultiplier[0] = sWorld->getRate(RATE_XP_KILL);
    DefaultExperienceMultiplier[1] = sWorld->getRate(RATE_XP_QUEST);
    DefaultExperienceMultiplier[2] = sWorld->getRate(RATE_XP_QUEST_DF);
    DefaultExperienceMultiplier[3] = sWorld->getRate(RATE_XP_EXPLORE);
    DefaultExperienceMultiplier[4] = sWorld->getRate(RATE_XP_PET);
    DefaultBattlegroundExperienceMultiplier[0] = sWorld->getRate(RATE_XP_BG_KILL_AV);
    DefaultBattlegroundExperienceMultiplier[1] = sWorld->getRate(RATE_XP_BG_KILL_WSG);
    DefaultBattlegroundExperienceMultiplier[2] = sWorld->getRate(RATE_XP_BG_KILL_AB);
    DefaultBattlegroundExperienceMultiplier[3] = sWorld->getRate(RATE_XP_BG_KILL_EOTS);
    DefaultBattlegroundExperienceMultiplier[4] = sWorld->getRate(RATE_XP_BG_KILL_SOTA);
    DefaultBattlegroundExperienceMultiplier[5] = sWorld->getRate(RATE_XP_BG_KILL_IC);
    DefaultMoneyMultiplier = sWorld->getRate(RATE_DROP_MONEY);
    DefaultProfessionsMultiplier[0] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_CRAFTING);
    DefaultProfessionsMultiplier[1] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_GATHERING);
    DefaultReputationMultiplier = sWorld->getRate(RATE_REPUTATION_GAIN);
    DefaultProficienciesMultiplier[0] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_DEFENSE);
    DefaultProficienciesMultiplier[1] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_WEAPON);
    DefaultHonorMultiplier = sWorld->getRate(RATE_HONOR);

    CheckFrequency = 10s;
    AnnouncementFrequency = 1h;
    m_BonusType = BONUS_NONE;
}

void WeekendBonus::SetRates(bool active)
{
    // calculate multipliers
    float xp_multiplier = ((active) ? ExperienceMultiplier : 1);
    float money_multiplier = ((active) ? MoneyMultiplier : 1);
    float profession_multiplier = ((active) ? ProfessionsMultiplier : 1);
    float rep_multiplier = ((active) ? ReputationMultiplier : 1);
    float proficiency_multiplier = ((active) ? ProficienciesMultiplier : 1);
    float hon_multiplier = ((active) ? HonorMultiplier : 1);

    // experience
    sWorld->setRate(RATE_XP_KILL, DefaultExperienceMultiplier[0] * xp_multiplier);
    sWorld->setRate(RATE_XP_QUEST, DefaultExperienceMultiplier[1] * xp_multiplier);
    sWorld->setRate(RATE_XP_QUEST_DF, DefaultExperienceMultiplier[2] * xp_multiplier);
    sWorld->setRate(RATE_XP_EXPLORE, DefaultExperienceMultiplier[3] * xp_multiplier);
    sWorld->setRate(RATE_XP_PET, DefaultExperienceMultiplier[4] * xp_multiplier);

    // battleground experience
    sWorld->setRate(RATE_XP_BG_KILL_AV, DefaultBattlegroundExperienceMultiplier[0] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_WSG, DefaultBattlegroundExperienceMultiplier[1] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_AB, DefaultBattlegroundExperienceMultiplier[2] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_EOTS, DefaultBattlegroundExperienceMultiplier[3] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_SOTA, DefaultBattlegroundExperienceMultiplier[4] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_IC, DefaultBattlegroundExperienceMultiplier[5] * xp_multiplier);

    // money
    sWorld->setRate(RATE_DROP_MONEY, DefaultMoneyMultiplier * money_multiplier);

    // professions
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_CRAFTING, DefaultProfessionsMultiplier[0] * profession_multiplier);
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_GATHERING, DefaultProfessionsMultiplier[1] * profession_multiplier);

    // reputation
    sWorld->setRate(RATE_REPUTATION_GAIN, DefaultReputationMultiplier * rep_multiplier);

    // skills
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_DEFENSE, DefaultProficienciesMultiplier[0] * proficiency_multiplier);
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_WEAPON, DefaultProficienciesMultiplier[1] * proficiency_multiplier);

    // honor
    sWorld->setRate(RATE_HONOR, DefaultHonorMultiplier * hon_multiplier);

    Triggered = active;
    AnnouncementTime = 0s;
}
