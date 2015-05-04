#pragma once

#include "aircraft.h"

#include <unordered_map>
#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{
    struct schedule
    {
        void set(const aircraft& aircraft_item, const boost::posix_time::ptime& landing_time);
        boost::posix_time::ptime get(const aircraft& aircraft_item) const;
    private:
        std::unordered_map<aircraft_id, boost::posix_time::ptime> storage_;
    };

    void print_schedule(const schedule& sched, const vector<aircraft>& aircrafts);
}