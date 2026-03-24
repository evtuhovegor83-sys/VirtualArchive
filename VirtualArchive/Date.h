#pragma once
#include <ctime>
#include <string>

class Date {
private:
    int day;
    int month;
    int year;

public:
    Date();
    Date(int d, int m, int y);
    Date(const Date& other);

    int getDay() const;
    int getMonth() const;
    int getYear() const;

    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);

    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>=(const Date& other) const;

    static Date now();

    std::string toString() const;
};