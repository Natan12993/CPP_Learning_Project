#pragma once

#include "aircraft_types.hpp"
#include "airport.hpp"

class AircraftFactory
{
private:
    std::unique_ptr<Aircraft> create_aircraft(const AircraftType& type, Airport* airport);
    std::set<std::string> aircraftsNumbers;

public:
    std::unique_ptr<Aircraft> create_random_aircraft(Airport* airport) {
        return create_aircraft(*(aircraft_types[rand() % 3]),airport);
    };
    void add_aircraft_number(const std::string& number){aircraftsNumbers.emplace(number);};
    std::set<std::string> get_aircraft_numbers() {return aircraftsNumbers;}
};