#include "stdafx.h"
#include "schedule.h"
#include "input_data.h"

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

    ptime schedule::get(const aircraft_id& id) const
    {
        return storage_.at(id);
    }

    void print_schedule(const schedule& sched, const vector<aircraft>& aircrafts, const input_data& data)
    {
        using namespace boost::posix_time;
        //ptime start = data.get_start_time().first;
        //for (auto& item : aircrafts)
        //{
        //    std::cerr << item.id().get_value() 
        //        << '\t' << (item.get_target_time() - start).total_seconds() / 60
        //        << '\t' << (item.get_min_time() - start).total_seconds() / 60
        //        << '\t' << (item.get_max_time() - start).total_seconds() / 60
        //        << '\t' << (sched.get(item) - start).total_seconds() / 60 << std::endl;
        //}
    }

}