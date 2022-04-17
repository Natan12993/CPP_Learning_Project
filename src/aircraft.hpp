#pragma once

#include "GL/displayable.hpp"
#include "aircraft_types.hpp"
#include "config.hpp"
#include "geometry.hpp"
#include "tower.hpp"
#include "waypoint.hpp"

#include <string>
#include <string_view>

class Aircraft : public GL::Displayable, public GL::DynamicObject
{
private:
    const AircraftType& type;
    const std::string flight_number;
    const int airline;
    Point3D pos, speed; // note: the speed should always be normalized to length 'speed'
    WaypointQueue waypoints = {};
    Tower& control;
    bool landing_gear_deployed = false; // is the landing gear deployed?
    bool is_at_terminal        = false;
    bool was_at_airport        = false;
    int fuel = (rand()%2850)+150;

    // turn the aircraft to arrive at the next waypoint
    // try to facilitate reaching the waypoint after the next by facing the
    // right way to this end,(!landing_gear_deployed) such that Z's distance to the next waypoint is
    // half our distance so: |w1 - pos| = d and [w1 - w2].normalize() = W and Z
    // = w1 + W*d/2
    void turn_to_waypoint();
    void turn(Point3D direction);

    // select the correct tile in the plane texture (series of 8 sprites facing
    // [North, NW, W, SW, S, SE, E, NE])
    unsigned int get_speed_octant() const;
    // when we arrive at a terminal, signal the tower
    void arrive_at_terminal();
    // deploy and retract landing gear depending on next waypoints
    void operate_landing_gear();

    template<bool front>
    void add_waypoint(const Waypoint& wp);

    float max_speed() const {return is_on_ground() ? type.max_ground_speed : type.max_air_speed; }

    Aircraft& operator=(const Aircraft&) = delete;

public:
    Aircraft(const AircraftType& type_, const std::string_view& flight_number_, const int airline_, const Point3D& pos_,
             const Point3D& speed_, Tower& control_) :
        GL::Displayable { pos_.x() + pos_.y() },
        type { type_ },
        flight_number { flight_number_ },
        airline {airline_},
        pos { pos_ },
        speed { speed_ },
        control { control_ }
    {
        speed.cap_length(max_speed());
    }

    int get_aircraft_airline() const { return airline; }
    const std::string& get_flight_num() const { return flight_number; }
    float distance_to(const Point3D& p) const { return pos.distance_to(p); }

    void display() const override;
    void move() override;

    bool del_object() const override
    {
        if(this->was_at_airport && !this->landing_gear_deployed)
        {
            return true;
        }
        if(this->fuel==0) 
        {
            std::cout << "Aircraft " << this->get_flight_num() << " has crashed" << std::endl;
            return true;
        }
        return false;
    }

    bool has_terminal() const;
    bool is_circling() const;

    bool operator<(const Aircraft&);
    bool is_low_on_fuel() const;

    int get_fuel(){return fuel;}
    bool at_terminal() const {return is_at_terminal;}
    void refill(unsigned int& fuel_stock);

    friend class Tower;

    Aircraft(const Aircraft&) = default;

    bool is_on_ground() const { return pos.z() < DISTANCE_THRESHOLD; }
};
