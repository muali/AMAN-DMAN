#include "schedule.h"

namespace AMAN
{
    void schedule::set(const aircraft& aircraft_item, const boost::posix_time::ptime& landing_time)
    {
        storage_[aircraft_item.id()] = landing_time;
    }

    boost::posix_time::ptime schedule::get(const aircraft& aircraft_item) const
    {
        return storage_.at(aircraft_item.id());
    }

}