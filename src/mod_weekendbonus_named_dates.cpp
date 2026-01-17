#include "mod_weekendbonus.h"

/*
    This module provides a way to check for named holidays and return their dates.
    The function CheckForNamedHoliday takes a holiday name as input and returns
    the corresponding date in MM/DD format. If the holiday name is not recognized,
    it returns an empty string.

    Additional named holidays can be added to the function as needed.  Most dates are
    prefixed with US_ to denote they are US holidays, but others like Christmas and Easter
    are more universal and do not have the prefix.

    Yes, it is a messy way to do it, but it gets the job done without
    requiring complex data structures or external libraries.
*/
std::string WeekendBonus::CheckForNamedHoliday(const std::string& holidayName)
{
    // Case-insensitive string comparison
    auto equalsIgnoreCase = [](const std::string& a, const std::string& b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                          [](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); });
    };

    m_NamedHoliday = holidayName;
    if (equalsIgnoreCase(holidayName, "US_New_Years_Day"))
    {
        return "01/01";
    }
    else if (equalsIgnoreCase(holidayName, "US_Christmas_Eve"))
    {
        return "12/24";
    }
    else if (equalsIgnoreCase(holidayName, "US_New_Years_Eve"))
    {
        return "12/31";
    }
    else if (equalsIgnoreCase(holidayName, "US_MLK_Day"))
    {
        // Martin Luther King Jr. Day is the third Monday in January
        int january = 1;
        int firstDayOfMonth = 1;

        struct tm firstOfJanuary = {};
        firstOfJanuary.tm_year = tm_LocalTime.tm_year; // current year
        firstOfJanuary.tm_mon = january - 1;            // zero-based month
        firstOfJanuary.tm_mday = firstDayOfMonth;

        mktime(&firstOfJanuary); // normalize to get the correct weekday

        int firstMondayOffset = (1 - firstOfJanuary.tm_wday + 7) % 7; // 1 = Monday
        int mlkDay = firstDayOfMonth + firstMondayOffset + 14; // third Monday

        return (january < 10 ? "0" : "") + std::to_string(january) + "/" +
               (mlkDay < 10 ? "0" : "") + std::to_string(mlkDay);
    }
    else if (equalsIgnoreCase(holidayName, "US_Presidents_Day"))
    {
        // Presidents' Day is the third Monday in February
        int february = 2;
        int firstDayOfMonth = 1;

        struct tm firstOfFebruary = {};
        firstOfFebruary.tm_year = tm_LocalTime.tm_year; // current year
        firstOfFebruary.tm_mon = february - 1;          // zero-based month
        firstOfFebruary.tm_mday = firstDayOfMonth;

        mktime(&firstOfFebruary); // normalize to get the correct weekday

        int firstMondayOffset = (1 - firstOfFebruary.tm_wday + 7) % 7; // 1 = Monday
        int presidentsDay = firstDayOfMonth + firstMondayOffset + 14; // third Monday

        return (february < 10 ? "0" : "") + std::to_string(february) + "/" +
               (presidentsDay < 10 ? "0" : "") + std::to_string(presidentsDay);
    }
    else if (equalsIgnoreCase(holidayName, "US_Valentines_Day"))
    {
        return "02/14";
    }
    else if (equalsIgnoreCase(holidayName, "US_Halloween"))
    {
        return "10/31";
    }
    else if (equalsIgnoreCase(holidayName, "US_Saint_Patricks_Day"))
    {
        return "03/17";
    }
    else if (equalsIgnoreCase(holidayName, "US_April_Fools_Day"))
    {
        return "04/01";
    }
    else if (equalsIgnoreCase(holidayName, "US_Mothers_Day"))
    {
        // Mother's Day is the second Sunday in May
        int may = 5;
        int firstDayOfMonth = 1;

        struct tm firstOfMay = {};
        firstOfMay.tm_year = tm_LocalTime.tm_year; // current year
        firstOfMay.tm_mon = may - 1;               // zero-based month
        firstOfMay.tm_mday = firstDayOfMonth;

        mktime(&firstOfMay); // normalize to get the correct weekday

        int firstSundayOffset = (0 - firstOfMay.tm_wday + 7) % 7; // 0 = Sunday
        int mothersDay = firstDayOfMonth + firstSundayOffset + 7; // second Sunday

        return (may < 10 ? "0" : "") + std::to_string(may) + "/" +
               (mothersDay < 10 ? "0" : "") + std::to_string(mothersDay);
    }
    else if (equalsIgnoreCase(holidayName, "US_Fathers_Day"))
    {
        // Father's Day is the third Sunday in June
        int june = 6;
        int firstDayOfMonth = 1;

        struct tm firstOfJune = {};
        firstOfJune.tm_year = tm_LocalTime.tm_year; // current year
        firstOfJune.tm_mon = june - 1;              // zero-based month
        firstOfJune.tm_mday = firstDayOfMonth;

        mktime(&firstOfJune); // normalize to get the correct weekday

        int firstSundayOffset = (0 - firstOfJune.tm_wday + 7) % 7; // 0 = Sunday
        int fathersDay = firstDayOfMonth + firstSundayOffset + 14; // third Sunday

        return (june < 10 ? "0" : "") + std::to_string(june) + "/" +
               (fathersDay < 10 ? "0" : "") + std::to_string(fathersDay);
    }
    else if (equalsIgnoreCase(holidayName, "US_Labor_Day"))
    {
        // Labor Day is the first Monday in September
        int september = 9;
        int firstDayOfMonth = 1;

        struct tm firstOfSept = {};
        firstOfSept.tm_year = tm_LocalTime.tm_year; // current year
        firstOfSept.tm_mon = september - 1;         // zero-based month
        firstOfSept.tm_mday = firstDayOfMonth;

        mktime(&firstOfSept); // normalize to get the correct weekday

        int firstMondayOffset = (1 - firstOfSept.tm_wday + 7) % 7; // 1 = Monday
        int laborDay = firstDayOfMonth + firstMondayOffset; // first Monday

        return (september < 10 ? "0" : "") + std::to_string(september) + "/" +
               (laborDay < 10 ? "0" : "") + std::to_string(laborDay);
    }
    else if (equalsIgnoreCase(holidayName, "US_Memorial_Day"))
    {
        // Memorial Day is the last Monday in May
        int may = 5;
        int lastDayOfMonth = 31;

        struct tm lastOfMay = {};
        lastOfMay.tm_year = tm_LocalTime.tm_year; // current year
        lastOfMay.tm_mon = may - 1;               // zero-based month
        lastOfMay.tm_mday = lastDayOfMonth;

        mktime(&lastOfMay); // normalize to get the correct weekday

        int lastMondayOffset = (lastOfMay.tm_wday - 1 + 7) % 7; // 1 = Monday
        int memorialDay = lastDayOfMonth - lastMondayOffset; // last Monday

        return (may < 10 ? "0" : "") + std::to_string(may) + "/" +
               (memorialDay < 10 ? "0" : "") + std::to_string(memorialDay);
    }
    else if (equalsIgnoreCase(holidayName, "US_Independence_Day"))
    {
        return "07/04";
    }
    else if (equalsIgnoreCase(holidayName, "US_Thanksgiving"))
    {
        // Thanksgiving is the fourth Thursday in November
        int november = 11;
        int firstDayOfMonth = 1;
        
        struct tm firstOfNov = {};
        firstOfNov.tm_year = tm_LocalTime.tm_year; // current year
        firstOfNov.tm_mon = november - 1;          // zero-based month
        firstOfNov.tm_mday = firstDayOfMonth;
        
        mktime(&firstOfNov); // normalize to get the correct weekday
        
        int firstThursdayOffset = (4 - firstOfNov.tm_wday + 7) % 7; // 4 = Thursday
        int thanksgivingDay = firstDayOfMonth + firstThursdayOffset + 21; // fourth Thursday
        
        return (november < 10 ? "0" : "") + std::to_string(november) + "/" +
            (thanksgivingDay < 10 ? "0" : "") + std::to_string(thanksgivingDay);
    }
    else if (equalsIgnoreCase(holidayName, "Christmas"))
    {
        return "12/25";
    }
    else if (equalsIgnoreCase(holidayName, "Easter"))
    {
        // Calculate Easter date using Anonymous Gregorian algorithm
        int year = tm_LocalTime.tm_year + 1900; // tm_year is years since 1900
        int a = year % 19;
        int b = year / 100;
        int c = year % 100;
        int d = b / 4;
        int e = b % 4;
        int f = (b + 8) / 25;
        int g = (b - f + 1) / 16;
        int h = (19 * a + b - d - g + 15) % 30;
        int i = c / 16;
        int k = c % 16;
        int l = (32 + 2 * e + 2 * i - h - k) % 7;
        int m = (a + 11 * h + 22 * l) / 451;
        int month = (h + l - 7 * m + 114) / 31; // March=3, April=4
        int day = ((h + l - 7 * m + 114) % 31) + 1;
    
        return (month < 10 ? "0" : "") + std::to_string(month) + "/" +
               (day < 10 ? "0" : "") + std::to_string(day);
    }
    /*else if (equalsIgnoreCase(holidayName, "TEST_TODAY"))
    {
        // For testing purposes, return today's date
        int month = tm_LocalTime.tm_mon + 1;
        int day = tm_LocalTime.tm_mday;
        return (month < 10 ? "0" : "") + std::to_string(month) + "/" +
               (day < 10 ? "0" : "") + std::to_string(day);
    }*/
    
    m_NamedHoliday = "";
    return "";
}
