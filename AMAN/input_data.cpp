#include "input_data.h"

namespace AMAN
{
    input_data::input_data(const boost::posix_time::ptime& freeze_time, uint8_t class_count)
        : separations_(class_count, std::vector<boost::posix_time::time_duration>(class_count))
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

    void input_data::add_aircraft(const aircraft& item)
    {
        aircrafts_.push_back(item);
    }

    std::vector<aircraft> input_data::get_unordered() const
    {
        return aircrafts_;
    }

}