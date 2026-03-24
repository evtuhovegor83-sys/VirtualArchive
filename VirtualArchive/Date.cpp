#define _CRT_SECURE_NO_WARNINGS

#include "Date.h"
#include <sstream>
#include <iomanip>

Date::Date() : day(1), month(1), year(2000) {}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

Date::Date(const Date& other) : day(other.day), month(other.month), year(other.year) {}

int Date::getDay() const { return day; }
int Date::getMonth() const { return month; }
int Date::getYear() const { return year; }

void Date::setDay(int d) { day = d; }
void Date::setMonth(int m) { month = m; }
void Date::setYear(int y) { year = y; }

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator>(const Date& other) const {
    if (year != other.year) return year > other.year;
    if (month != other.month) return month > other.month;
    return day > other.day;
}

bool Date::operator==(const Date& other) const {
    return day == other.day && month == other.month && year == other.year;
}

bool Date::operator<=(const Date& other) const {
    return (*this < other) || (*this == other);
}

bool Date::operator>=(const Date& other) const {
    return (*this > other) || (*this == other);
}

Date Date::now() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    return Date(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "."
        << std::setw(2) << month << "." << year;
    return oss.str();
}