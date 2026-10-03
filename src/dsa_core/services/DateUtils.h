#pragma once

#include <stdexcept>

// Convert YYYYMMDD to a consecutive Gregorian day number.
inline int dateKeyToDayNumber(int dateKey)
{
    int year = dateKey / 10000;
    const int month = (dateKey / 100) % 100;
    const int day = dateKey % 100;
    if (month < 1 || month > 12 || day < 1)
        throw std::invalid_argument("Invalid YYYYMMDD date key");

    const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    const int monthLengths[] = {31, leap ? 29 : 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31};
    if (day > monthLengths[month - 1])
        throw std::invalid_argument("Invalid YYYYMMDD date key");

    year -= month <= 2;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yearOfEra = static_cast<unsigned>(year - era * 400);
    const unsigned adjustedMonth = static_cast<unsigned>(month + (month > 2 ? -3 : 9));
    const unsigned dayOfYear = (153 * adjustedMonth + 2) / 5 +
                               static_cast<unsigned>(day - 1);
    const unsigned dayOfEra = yearOfEra * 365 + yearOfEra / 4 -
                              yearOfEra / 100 + dayOfYear;
    return era * 146097 + static_cast<int>(dayOfEra);
}
