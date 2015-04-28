#include "stdafx.h"
#include "input_data.h"

namespace AMAN
{
    input_data::input_data(const boost::posix_time::ptime& freeze_time, uint8_t class_count)
        : separations_(class_count, std::vector<boost::posix_time::time_duration>(class_count))
        , start_time_(boost::posix_time::second_clock::local_time())
    {
    }

    void input_data::set_separation(uint8_t class_last, uint8_t class_next, 
        const boost::posix_time::time_duration& separation)
    {
        separations_[class_last][class_next] = separation;
    }

    boost::posix_time::time_duration input_data::get_separation(const aircraft& last, const aircraft& next) const
    {
        return separations_[last.get_class()][next.get_class()];
    }

    boost::posix_time::time_duration input_data::get_separation(uint8_t class_last, uint8_t class_next) const
    {
        return separations_[class_last][class_next];
    }

    boost::posix_time::ptime input_data::get_start_time() const
    {
        //TO DO: use more significant value (last seating time of frozen aircraft)
        return start_time_;
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