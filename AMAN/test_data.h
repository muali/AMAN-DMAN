#pragma once

#include "aircraft.h"
#include "input_data.h"
#include "schedule.h"

#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{

struct simulated_aircraft : aircraft
{
    simulated_aircraft(const aircraft_id& id,
        uint8_t turbulence_class,
        const boost::posix_time::ptime& min_time,
        const boost::posix_time::ptime& max_time,
        const boost::posix_time::ptime& target_time,
        double cost_per_minute_before,
        double cost_per_minute_after);

    void set_min_time(const boost::posix_time::ptime& min_time);
};

struct test_data
{
    test_data(const boost::posix_time::time_duration& freeze_time, 
        const boost::posix_time::ptime& start_time, uint8_t class_count);

    void set_separation(uint8_t class_last, uint8_t class_next, const boost::posix_time::time_duration& separation);
    void add_aircraft(const simulated_aircraft& item, boost::posix_time::ptime& appearence_time);

    input_data next();
    bool has_next();
    schedule merge(const schedule& new_schedule);

private:
    std::vector<std::pair<boost::posix_time::ptime, simulated_aircraft>> aircrafts_;
    std::vector<std::vector<boost::posix_time::time_duration>> separations_;
    boost::posix_time::time_duration freeze_time_;
    boost::posix_time::ptime current_time_;
    schedule current_schedule_;
};

} //AMAN