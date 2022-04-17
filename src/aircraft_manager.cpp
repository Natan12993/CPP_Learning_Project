#include "aircraft_manager.hpp"
#include <algorithm>


void AircraftManager::move() {

    std::sort(aircrafts.begin(), aircrafts.end(), [](const std::unique_ptr<Aircraft>& a, const std::unique_ptr<Aircraft>& b) {return a < b;});

    std::function<bool(std::unique_ptr<Aircraft>&)> is_delete = [this](std::unique_ptr<Aircraft>& aircraft){
        try {
            aircraft->move();
        }catch(const AircraftCrash& crash){
            std::cerr << crash.what() << std::endl;
            ++total_crashed_aircraft;
        }
         return aircraft->del_object();
    };
    
    aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(), is_delete),aircrafts.end());
    
    
    /*for (auto it = aircrafts.begin(); it != aircrafts.end();) {
        auto& aircraft = **it;
        aircraft.move();

        if ((aircraft).del_object()) {
            it = aircrafts.erase(it);

        } else {
            ++it;
        }
    }*/
}

void AircraftManager::add_aircraft(std::unique_ptr<Aircraft> aircraft){
    aircrafts.emplace_back(std::move(aircraft));
}

void AircraftManager::aircrafts_by_airlines(int airline)
{
    assert(airline >= 0 && airline <= 7);
    std::cout << std::count_if(aircrafts.begin(), aircrafts.end(), [airline](const std::unique_ptr<Aircraft>& aircraft) {return aircraft->get_aircraft_airline() == airline;}) << std::endl;
}


int AircraftManager::get_required_fuel()
{
    return std::accumulate(aircrafts.begin(), aircrafts.end(), 0, [](int sum, const std::unique_ptr<Aircraft>& aircraft)
    {
        assert(aircraft != nullptr);
        if (aircraft->is_on_ground() && aircraft->is_low_on_fuel()) {
            return sum + (3000 - aircraft->get_fuel());
        }
        return sum;
    });
}
