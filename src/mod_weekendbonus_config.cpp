#include "mod_weekendbonus.h"

#include "Config.h"

#include <sstream>
#include <vector>
#include <string>

void WeekendBonus::OnAfterConfigLoad(bool reload)
{
    LOG_INFO("weekendbonus", "> WeekendBonus Loading Configuration");

    UpdateLocalTime();

    // Helper function to split a string by a delimiter
    auto stringSplit = [](const std::string& str, char delimiter) {
        std::vector<std::string> elements;
        std::string item;
        std::istringstream stream(str);
        while (std::getline(stream, item, delimiter))
        {
            elements.push_back(item);
        }
        return elements;
    };

    if (!reload)
    {
        LoadDefaultValues();
    }

    // weekend multipliers
    m_ExperienceMultiplier[BM_WEEKEND] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Experience", 2.0f);
    m_MoneyMultiplier[BM_WEEKEND] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Money", 2.0f);
    m_ProfessionsMultiplier[BM_WEEKEND] = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Professions", 2);
    m_ReputationMultiplier[BM_WEEKEND] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Reputation", 2.0f);
    m_ProficienciesMultiplier[BM_WEEKEND] = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Proficiencies", 2);
    m_HonorMultiplier[BM_WEEKEND] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Honor", 2.0f);

    // evening multipliers
    m_EveningEnabled = sConfigMgr->GetOption<bool>("WeekendBonus.Evening.Bonuses.Enabled", 0);

    // 18:00:00 to 22:00:00 (6 PM to 10 PM) is considered the default evening period, if not otherwise specified in the configuration
    m_EveningStart = sConfigMgr->GetOption<int>("WeekendBonus.Evening.Start", 180000);
    m_EveningEnd = sConfigMgr->GetOption<int>("WeekendBonus.Evening.End", 220000);

    m_ExperienceMultiplier[BM_EVENING] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Evening.Experience", 2.0f);
    m_MoneyMultiplier[BM_EVENING] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Evening.Money", 2.0f);
    m_ProfessionsMultiplier[BM_EVENING] = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Evening.Professions", 2);
    m_ReputationMultiplier[BM_EVENING] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Evening.Reputation", 2.0f);
    m_ProficienciesMultiplier[BM_EVENING] = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Evening.Proficiencies", 2);
    m_HonorMultiplier[BM_EVENING] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Evening.Honor", 2.0f);

    // holiday multipliers
    m_HolidayEnabled = sConfigMgr->GetOption<bool>("WeekendBonus.Holiday.Bonuses.Enabled", 0);
    std::string holidayListStr = sConfigMgr->GetOption<std::string>("WeekendBonus.Holiday.Bonuses.Holidays", "");
    m_ExperienceMultiplier[BM_HOLIDAY] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Holiday.Experience", 2.0f);
    m_MoneyMultiplier[BM_HOLIDAY] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Holiday.Money", 2.0f);
    m_ProfessionsMultiplier[BM_HOLIDAY] = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Holiday.Professions", 2);
    m_ReputationMultiplier[BM_HOLIDAY] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Holiday.Reputation", 2.0f);
    m_ProficienciesMultiplier[BM_HOLIDAY] = sConfigMgr->GetOption<uint32>("WeekendBonus.Multiplier.Holiday.Proficiencies", 2);
    m_HonorMultiplier[BM_HOLIDAY] = sConfigMgr->GetOption<float>("WeekendBonus.Multiplier.Holiday.Honor", 2.0f);

#if WEEKENDBONUS_DEBUG
    LOG_INFO("weekendbonus", "> Holiday List: {}", holidayListStr);
#endif

    m_HolidayDates.clear();
    if (!holidayListStr.empty())
    {
        // dates will be delimited by commas, each date in MM/DD format
        for (const std::string& dateStr : stringSplit(holidayListStr, ','))
        {
#if WEEKENDBONUS_DEBUG
    LOG_INFO("weekendbonus", "> Date: {}", dateStr);
#endif
            int month = 0;
            int day = 0;
            size_t sep = dateStr.find('/');
            if (sep == std::string::npos) {
                std::string namedDate = CheckForNamedHoliday(dateStr);
                if (namedDate.empty()) {
#if WEEKENDBONUS_DEBUG
    LOG_INFO("weekendbonus", "> Date: {}", " - unrecognized named holiday");
#endif
                    continue; // unrecognized named holiday
                }
                else
                {
#if WEEKENDBONUS_DEBUG
    LOG_INFO("weekendbonus", "> Named MM/DD: {}", namedDate);
#endif
                    month = std::stoi(namedDate.substr(0, 2));
                    day = std::stoi(namedDate.substr(3, 2));
                }
            }
            else
            {
#if WEEKENDBONUS_DEBUG
    LOG_INFO("weekendbonus", "> MM/DD: {}", dateStr);
#endif
                month = std::stoi(dateStr.substr(0, sep));
                day = std::stoi(dateStr.substr(sep + 1));
            }

            // validate month/day are in valid ranges
            if (month >= 1 && month <= 12 && day >= 1 && day <= 31)
            {
                m_HolidayDates.emplace_back(month, day);
            }
        }
    }

    if (reload)
    {
        m_BonusType = GetCurrentBonusType();
        SetRates(m_BonusType != BONUS_NONE);
    }
}

void WeekendBonus::LoadDefaultValues()
{
    m_DefaultExperienceMultiplier[0] = sWorld->getRate(RATE_XP_KILL);
    m_DefaultExperienceMultiplier[1] = sWorld->getRate(RATE_XP_QUEST);
    m_DefaultExperienceMultiplier[2] = sWorld->getRate(RATE_XP_QUEST_DF);
    m_DefaultExperienceMultiplier[3] = sWorld->getRate(RATE_XP_EXPLORE);
    m_DefaultExperienceMultiplier[4] = sWorld->getRate(RATE_XP_PET);
    m_DefaultBattlegroundExperienceMultiplier[0] = sWorld->getRate(RATE_XP_BG_KILL_AV);
    m_DefaultBattlegroundExperienceMultiplier[1] = sWorld->getRate(RATE_XP_BG_KILL_WSG);
    m_DefaultBattlegroundExperienceMultiplier[2] = sWorld->getRate(RATE_XP_BG_KILL_AB);
    m_DefaultBattlegroundExperienceMultiplier[3] = sWorld->getRate(RATE_XP_BG_KILL_EOTS);
    m_DefaultBattlegroundExperienceMultiplier[4] = sWorld->getRate(RATE_XP_BG_KILL_SOTA);
    m_DefaultBattlegroundExperienceMultiplier[5] = sWorld->getRate(RATE_XP_BG_KILL_IC);
    m_DefaultMoneyMultiplier = sWorld->getRate(RATE_DROP_MONEY);
    m_DefaultProfessionsMultiplier[0] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_CRAFTING);
    m_DefaultProfessionsMultiplier[1] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_GATHERING);
    m_DefaultReputationMultiplier = sWorld->getRate(RATE_REPUTATION_GAIN);
    m_DefaultProficienciesMultiplier[0] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_DEFENSE);
    m_DefaultProficienciesMultiplier[1] = sWorld->getIntConfig(CONFIG_SKILL_GAIN_WEAPON);
    m_DefaultHonorMultiplier = sWorld->getRate(RATE_HONOR);

    CheckFrequency = 10s;
    AnnouncementFrequency = 1h;
    m_BonusType = BONUS_NONE;
}

void WeekendBonus::SetRates(bool active)
{
    int idx = static_cast<int>(m_BonusMultiplier);
    
    // calculate multipliers
    float xp_multiplier = ((active) ? m_ExperienceMultiplier[idx] : 1);
    float money_multiplier = ((active) ? m_MoneyMultiplier[idx] : 1);
    float profession_multiplier = ((active) ? m_ProfessionsMultiplier[idx] : 1);
    float rep_multiplier = ((active) ? m_ReputationMultiplier[idx] : 1);
    float proficiency_multiplier = ((active) ? m_ProficienciesMultiplier[idx] : 1);
    float hon_multiplier = ((active) ? m_HonorMultiplier[idx] : 1);

    // experience
    sWorld->setRate(RATE_XP_KILL, m_DefaultExperienceMultiplier[0] * xp_multiplier);
    sWorld->setRate(RATE_XP_QUEST, m_DefaultExperienceMultiplier[1] * xp_multiplier);
    sWorld->setRate(RATE_XP_QUEST_DF, m_DefaultExperienceMultiplier[2] * xp_multiplier);
    sWorld->setRate(RATE_XP_EXPLORE, m_DefaultExperienceMultiplier[3] * xp_multiplier);
    sWorld->setRate(RATE_XP_PET, m_DefaultExperienceMultiplier[4] * xp_multiplier);

    // battleground experience
    sWorld->setRate(RATE_XP_BG_KILL_AV, m_DefaultBattlegroundExperienceMultiplier[0] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_WSG, m_DefaultBattlegroundExperienceMultiplier[1] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_AB, m_DefaultBattlegroundExperienceMultiplier[2] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_EOTS, m_DefaultBattlegroundExperienceMultiplier[3] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_SOTA, m_DefaultBattlegroundExperienceMultiplier[4] * xp_multiplier);
    sWorld->setRate(RATE_XP_BG_KILL_IC, m_DefaultBattlegroundExperienceMultiplier[5] * xp_multiplier);

    // money
    sWorld->setRate(RATE_DROP_MONEY, m_DefaultMoneyMultiplier * money_multiplier);

    // professions
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_CRAFTING, m_DefaultProfessionsMultiplier[0] * profession_multiplier);
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_GATHERING, m_DefaultProfessionsMultiplier[1] * profession_multiplier);

    // reputation
    sWorld->setRate(RATE_REPUTATION_GAIN, m_DefaultReputationMultiplier * rep_multiplier);

    // skills
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_DEFENSE, m_DefaultProficienciesMultiplier[0] * proficiency_multiplier);
    sWorld->setIntConfig(CONFIG_SKILL_GAIN_WEAPON, m_DefaultProficienciesMultiplier[1] * proficiency_multiplier);

    // honor
    sWorld->setRate(RATE_HONOR, m_DefaultHonorMultiplier * hon_multiplier);

    Triggered = active;
    AnnouncementTime = 0s;
}
