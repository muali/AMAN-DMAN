#include "stdafx.h"
#include "input_data.h"

namespace AMAN
{

input_data::input_data(const ptime& start_time, uint8_t class_prev, uint8_t class_count)
    : separations_(class_count, vector<time_duration>(class_count))
    , start_time_(start_time)
    , class_prev_(class_prev)
{
}

void input_data::set_separation(uint8_t class_last, uint8_t class_next, 
    const time_duration& separation)
{
    separations_[class_last][class_next] = separation;
}

time_duration input_data::get_separation(const aircraft& last, const aircraft& next) const
{
    return separations_[last.get_class()][next.get_class()];
}

time_duration input_data::get_separation(uint8_t class_last, uint8_t class_next) const
{
    return separations_[class_last][class_next];
}

ptime input_data::get_start_time(uint8_t class_first) const
{
    if (class_prev_ < separations_.size())
        return start_time_ + separations_[class_prev_][class_first];
    return start_time_;
}

std::pair<boost::posix_time::ptime, uint8_t> input_data::get_start_time() const
{
    return std::make_pair(start_time_, class_prev_);
}

void input_data::add_aircraft(const aircraft& item)
{
    aircrafts_.push_back(item);
}

std::vector<aircraft> input_data::get_unordered() const
{
    return aircrafts_;
}

}