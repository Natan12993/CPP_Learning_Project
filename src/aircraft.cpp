#include "aircraft.hpp"

#include "GL/opengl_interface.hpp"

#include <cmath>

void Aircraft::turn_to_waypoint()
{
    if (!waypoints.empty())
    {
        Point3D target = waypoints[0];
        if (waypoints.size() > 1)
        {
            const float d   = (waypoints[0] - pos).length();
            const Point3D W = (waypoints[0] - waypoints[1]).normalize(d / 2.0f);
            target += W;
        }

        turn(target - pos - speed);
    }
}

void Aircraft::turn(Point3D direction)
{
    (speed += direction.cap_length(type.max_accel)).cap_length(max_speed());
}

unsigned int Aircraft::get_speed_octant() const
{
    const float speed_len = speed.length();
    if (speed_len > 0)
    {
        const Point3D norm_speed { speed * (1.0f / speed_len) };
        const float angle =
            (norm_speed.y() > 0) ? 2.0f * 3.141592f - std::acos(norm_speed.x()) : std::acos(norm_speed.x());
        // partition into NUM_AIRCRAFT_TILES equal pieces
        return (static_cast<int>(std::round((angle * NUM_AIRCRAFT_TILES) / (2.0f * 3.141592f))) + 1) %
               NUM_AIRCRAFT_TILES;
    }
    else
    {
        return 0;
    }
}

// when we arrive at a terminal, signal the tower
void Aircraft::arrive_at_terminal()
{
    // we arrived at a terminal, so start servicing
    control.arrived_at_terminal(*this);
    is_at_terminal = true;
    was_at_airport = true;
}

// deploy and retract landing gear depending on next waypoints
void Aircraft::operate_landing_gear()
{
    if (waypoints.size() > 1u)
    {
        const auto it            = waypoints.begin();
        const bool ground_before = it->is_on_ground();
        const bool ground_after  = std::next(it)->is_on_ground();
        // deploy/retract landing gear when landing/lifting-off
        if (ground_before && !ground_after)
        {
            std::cout << flight_number << " lift off" << std::endl;
        }
        else if (!ground_before && ground_after)
        {
            std::cout << flight_number << " is now landing..." << std::endl;
            landing_gear_deployed = true;
        }
        else if (!ground_before && !ground_after)
        {
            landing_gear_deployed = false;
        }
    }
}

template <bool front>
void Aircraft::add_waypoint(const Waypoint& wp)
{
    if constexpr (front)
    {
        waypoints.push_front(wp);
    }
    else
    {
        waypoints.push_back(wp);
    }
}

void Aircraft::move()
{
    this->fuel--;

    if(fuel <= 0)
    {
        throw AircraftCrash {flight_number + " out of fuel then crashed " };
    }

    if (waypoints.empty())
    {
        //auto front = false;
        for (const auto& wp: control.get_instructions(*this))
        {
            add_waypoint<false>(wp);
        }
    }

    if (!is_at_terminal)
    {
        if(is_circling())
        {
            auto path = control.reserve_terminal(*this);
            if(!path.empty())
            {
                waypoints = std::move(path);
            }
        }

        turn_to_waypoint();
        // move in the direction of the current speed
        pos += speed;

        // if we are close to our next waypoint, stike if off the list
        if (!waypoints.empty() && distance_to(waypoints.front()) < DISTANCE_THRESHOLD)
        {

            //std::cout << "avant : " << distance_to(waypoints.front()) << "  pos: " << waypoints.front() << std::endl;

            if (waypoints.front().is_at_terminal())
            {

                arrive_at_terminal();
            }
            else
            {
                operate_landing_gear();
            }
            waypoints.pop_front();
        }

        if (is_on_ground())
        {
            if (!landing_gear_deployed)
            {
                using namespace std::string_literals;
                throw AircraftCrash { flight_number + " crashed into the ground" };
            }
        }
        else
        {
            // if we are in the air, but too slow, then we will sink!
            const float speed_len = speed.length();
            if (speed_len < SPEED_THRESHOLD)
            {
                pos.z() -= SINK_FACTOR * (SPEED_THRESHOLD - speed_len);
            }
        }

        // update the z-value of the displayable structure
        GL::Displayable::z = pos.x() + pos.y();
    }
}

void Aircraft::display() const
{
    type.texture.draw(project_2D(pos), { PLANE_TEXTURE_DIM, PLANE_TEXTURE_DIM }, get_speed_octant());
}




bool Aircraft::has_terminal() const
{
    if(control.get_reserved_terminal().find(this) != control.get_reserved_terminal().end()){
        return true;
    }
    return false;
}

bool Aircraft::is_circling() const
{
    return !has_terminal();
}

bool Aircraft::operator<(const Aircraft& other)
{
    if (has_terminal())
    {
        if(other.has_terminal())
        {
            return fuel < other.fuel;
        }
        return true;
    }
    else
    {
        if(other.has_terminal())
        {
            return false;
        }
        return fuel < other.fuel;
    }
}

bool Aircraft::is_low_on_fuel() const {
    if(this->fuel < 200)
    {
        return true;
    }
    return false;
}

void Aircraft::refill(unsigned int &fuel_stock) {
    unsigned int need_fuel = 3000 - fuel;
    unsigned int fuel_loaded = fuel_stock > need_fuel ? need_fuel : fuel_stock;
    if(fuel_stock > 0)
    {
        fuel_stock -= fuel_loaded;
        fuel += fuel_loaded;
        std::cout << "Aircraft " << flight_number << " is being refilled with " << fuel_loaded << " of fuel." << std::endl;
    }
}

