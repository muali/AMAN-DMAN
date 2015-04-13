#include "aircraft.h"

#include <limits>

namespace AMAN
{
    aircraft_id aircraft_id::from_idx(size_t idx)
    {
        return aircraft_id(idx);
    }
    //-------------------------------------------------------------------------------------

    aircraft_id::aircraft_id(size_t value)
        : value_(value)
    {

    }
    //-------------------------------------------------------------------------------------

    aircraft::aircraft(uint8_t turbulence_class,
        const boost::posix_time::ptime& min_time,
        const boost::posix_time::ptime& max_time,
        const boost::posix_time::ptime& appearance_time,
        const boost::posix_time::ptime& target_time,
        double cost_per_minute_before,
        double cost_per_minute_after)
        : turbulence_class_(turbulence_class)
        , min_time_(min_time)
        , max_time_(max_time)
        , appearance_time_(appearance_time)
        , target_time_(target_time)
        , cost_per_second_before_(cost_per_minute_before / 60.)
        , cost_per_second_after_(cost_per_minute_after / 60.)
    {

    }
    //-------------------------------------------------------------------------------------

    double aircraft::estimate_cost(const boost::posix_time::ptime& landing_time) const
    {
        if (!is_valid_landing_time(landing_time))
            return std::numeric_limits<double>::infinity();
        boost::posix_time::time_duration dif = target_time_ - landing_time;
        int32_t seconds_dif = dif.seconds();
        if (seconds_dif < 0)
        {
            return -cost_per_second_before_ * seconds_dif;
        }
        else
        {
            return cost_per_second_after_ * seconds_dif;
        }
    }
    //-------------------------------------------------------------------------------------

    bool aircraft::is_valid_landing_time(const boost::posix_time::ptime& landing_time) const
    {
        return landing_time >= min_time_ && landing_time <= max_time_;
    }
    //-------------------------------------------------------------------------------------

    double aircraft::get_cost_per_second_before() const
    {
        return cost_per_second_before_;
    }
    //-------------------------------------------------------------------------------------

    double aircraft::get_cost_per_second_after() const
    {
        return cost_per_second_after_;
    }
    //-------------------------------------------------------------------------------------

    boost::posix_time::ptime aircraft::get_min_time() const
    {
        return min_time_;
    }
    //-------------------------------------------------------------------------------------

    boost::posix_time::ptime aircraft::get_max_time() const
    {
        return max_time_;
    }
    //-------------------------------------------------------------------------------------

    boost::posix_time::ptime aircraft::get_appearance_time() const
    {
        return appearance_time_;
    }
    //-------------------------------------------------------------------------------------

    boost::posix_time::ptime aircraft::get_target_time() const
    {
        return target_time_;
    }
    //-------------------------------------------------------------------------------------

    uint8_t aircraft::get_class() const
    {
        return turbulence_class_;
    }
} //AMAN