#pragma once

#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{
    struct aircraft_id
    {
        static aircraft_id from_idx(size_t idx);
        size_t hash() const;
        bool operator== (const aircraft_id& rhs) const;
        size_t get_value() const;
    private:
        aircraft_id(size_t value);
        size_t value_;
    };
    //-------------------------------------------------------------------------------------

    struct aircraft
    {
        aircraft(const aircraft_id& id,
            uint8_t turbulence_class, 
            const boost::posix_time::ptime& min_time,
            const boost::posix_time::ptime& max_time,
            const boost::posix_time::ptime& appearance_time,
            const boost::posix_time::ptime& target_time,
            double cost_per_minute_before,
            double cost_per_minute_after);
        
        aircraft_id id() const;
        double estimate_cost(const boost::posix_time::ptime& landing_time) const;
        bool is_valid_landing_time(const boost::posix_time::ptime& landing_time) const;
        double get_cost_per_second_before() const;
        double get_cost_per_second_after() const;
        boost::posix_time::ptime get_min_time() const;
        boost::posix_time::ptime get_max_time() const;
        boost::posix_time::ptime get_appearance_time() const;
        boost::posix_time::ptime get_target_time() const;
        uint8_t get_class() const;

    private:
        aircraft_id id_;
        uint8_t turbulence_class_;
        boost::posix_time::ptime min_time_;
        boost::posix_time::ptime max_time_;
        boost::posix_time::ptime appearance_time_;
        boost::posix_time::ptime target_time_;
        double cost_per_second_before_;
        double cost_per_second_after_;
    };

} // AMAN

namespace std
{
    template <>
    struct hash < AMAN::aircraft_id >
    {
        size_t operator()(const AMAN::aircraft_id& key_val) const
        {
            return key_val.hash();
        }
    };
} // std