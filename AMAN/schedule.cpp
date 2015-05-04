#include "stdafx.h"
#include "schedule.h"

namespace AMAN
{
    void schedule::set(const aircraft& aircraft_item, const ptime& landing_time)
    {
        storage_[aircraft_item.id()] = landing_time;
    }

    ptime schedule::get(const aircraft& aircraft_item) const
    {
        return storage_.at(aircraft_item.id());
    }

    void print_schedule(const schedule& sched, const vector<aircraft>& aircrafts)
    {
        using boost::posix_time::to_simple_string;
        for (auto& item : aircrafts)
        {
            std::cerr << item.id().get_value() 
                << '\t' << to_simple_string(item.get_target_time()) 
                << '\t' << to_simple_string(item.get_min_time())
                << '\t' << to_simple_string(item.get_max_time())
                << '\t' << to_simple_string(sched.get(item)) << std::endl;
        }
    }

}