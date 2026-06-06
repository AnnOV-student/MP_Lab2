#pragma once
#include <string>
#include <tuple>

struct Passenger {
    std::string fio;
    int cabinNumber;
    std::string cabinType;
    std::string destinationPort;

    bool operator<(const Passenger& other) const {
        return std::tie(cabinNumber, destinationPort, fio) < std::tie(other.cabinNumber, other.destinationPort, other.fio);
    }
    bool operator>(const Passenger& other) const {
        return std::tie(cabinNumber, destinationPort, fio) > std::tie(other.cabinNumber, other.destinationPort, other.fio);
    }
    bool operator<=(const Passenger& other) const {
        return !(*this > other);
    }
    bool operator>=(const Passenger& other) const {
        return !(*this < other);
    }
};