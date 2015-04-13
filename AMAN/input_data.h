#pragma once

#include "aircraft.h"

#include <unordered_map>
#include <vector>
#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{
    struct input_data
    {
        input_data(const boost::posix_time::ptime& freeze_time, uint8_t class_count);
        
        void set_separation(uint8_t class_last, uint8_t class_next, const boost::posix_time::time_duration& separation);
        boost::posix_time::time_duration get_separation(const aircraft& last, const aircraft& next) const;

        void add_aircraft(const aircraft& item);
        std::vector<aircraft> get_unordered() const;

    private:
        std::vector<aircraft> aircrafts_;
        std::vector<std::vector<boost::posix_time::time_duration>> separations_;
    };
}