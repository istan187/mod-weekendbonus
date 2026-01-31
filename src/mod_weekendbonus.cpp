#include "mod_weekendbonus.h"

WeekendBonus::WeekendBonus() : PlayerScript("WeekendBonusPlayerScript"), WorldScript("WeekendBonusWorldScript"), m_CurrentHoliday(0, 0, "") {}

void Addmod_weekendbonusScripts()
{
    new WeekendBonus();
}
