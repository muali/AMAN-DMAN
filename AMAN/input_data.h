#pragma once

#include "aircraft.h"

#include <unordered_map>
#include <vector>
#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{

struct input_data
{
    input_data(const boost::posix_time::ptime& start_time, uint8_t class_prev, uint8_t class_count);
        
    void set_separation(uint8_t class_last, uint8_t class_next, const boost::posix_time::time_duration& separation);
    boost::posix_time::time_duration get_separation(const aircraft& last, const aircraft& next) const;
    boost::posix_time::time_duration get_separation(uint8_t class_last, uint8_t class_next) const;

    void add_aircraft(const aircraft& item);
    std::vector<aircraft> get_unordered() const;

    boost::posix_time::ptime get_start_time(uint8_t class_first) const;

private:
    std::vector<aircraft> aircrafts_;
    std::vector<std::vector<boost::posix_time::time_duration>> separations_;
    boost::posix_time::ptime start_time_;
    uint8_t class_prev_;
};

} //AMAN