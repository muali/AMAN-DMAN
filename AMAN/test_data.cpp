#include "stdafx.h"

#include "test_data.h"

namespace AMAN
{

simulated_aircraft::simulated_aircraft(const aircraft_id& id,
    uint8_t turbulence_class,
    const boost::posix_time::ptime& min_time,
    const boost::posix_time::ptime& max_time,
    const boost::posix_time::ptime& target_time,
    double cost_per_minute_before,
    double cost_per_minute_after)
    : aircraft(id, turbulence_class, min_time, max_time, target_time, cost_per_minute_before, cost_per_minute_after)
{
}

void simulated_aircraft::set_min_time(const ptime& min_time)
{
    min_time_ = min_time;
}

test_data::test_data(const time_duration& freeze_time, const ptime& start_time, uint8_t class_count)
    : separations_(class_count, vector<time_duration>(class_count))
    , freeze_time_(freeze_time)
    , current_time_(start_time - minutes(1))
    , next_time_(start_time - minutes(1))
{
}

void test_data::set_separation(uint8_t class_last, uint8_t class_next, const time_duration& separation)
{
    separations_[class_last][class_next] = separation;
}

void test_data::add_aircraft(const simulated_aircraft& item, ptime& appearence_time)
{
    aircrafts_.push_back(std::make_pair(appearence_time, item));
}

input_data test_data::next()
{
    current_time_ = next_time_;
    next_time_ = ptime(boost::posix_time::max_date_time);
    for (auto item : aircrafts_)
    {
        if (item.first > current_time_)
            next_time_ = std::min(next_time_, item.first);
    }

    ptime start_time(current_time_);
    uint8_t class_last = separations_.size();
    for (auto item : aircrafts_)
    {
        if (item.first <= current_time_ && current_schedule_.get(item.second) < next_time_ + freeze_time_
            && (start_time < current_schedule_.get(item.second) || class_last == separations_.size()))
        {
            start_time = current_schedule_.get(item.second);
            class_last = item.second.get_class();
        }
    }

    input_data result(start_time, class_last, separations_.size());
    for (uint8_t i = 0; i < separations_.size(); ++i)
    {
        for (uint8_t j = 0; j < separations_[i].size(); ++j)
        {
            result.set_separation(i, j, separations_[i][j]);
        }
    }

    for (auto item : aircrafts_)
    {
        if (item.first <= current_time_ && current_schedule_.get(item.second) < next_time_ + freeze_time_)
            continue;
        if (item.first <= next_time_)
        {
            item.second.set_min_time(std::max(item.second.get_min_time(), next_time_ + freeze_time_));
            result.add_aircraft(item.second);
        }
    }

    return result;
}

bool test_data::has_next()
{
    for (auto item : aircrafts_)
    {
        if (item.first > next_time_)
            return true;
    }
    return false;
}

schedule test_data::merge(const schedule& new_schedule)
{
    for (auto item : aircrafts_)
    {
        if (item.first <= next_time_)
        {
            if (item.first <= current_time_ && current_schedule_.get(item.second) > next_time_ + freeze_time_
                || item.first > current_time_)
            {
                current_schedule_.set(item.second, new_schedule.get(item.second));
            }
        }
    }
    return current_schedule_;
}

double test_data::estimate()
{
    //TO DO: current implementation can accept some schedule, which violate freeze_time constraint
    double total_cost = 0.;
    vector<aircraft> sequence;
    for (size_t i = 0; i < aircrafts_.size(); ++i)
    {
        sequence.push_back(aircrafts_[i].second);
    }
    sort(sequence.begin(), sequence.end(), [&](const aircraft& a, const aircraft& b)
    {
        return current_schedule_.get(a) < current_schedule_.get(b);
    });

    for (size_t i = 0; i < sequence.size(); ++i)
    {
        total_cost += sequence[i].estimate_cost(current_schedule_.get(sequence[i]));
        for (size_t j = 0; j < i; ++j)
        {
            time_duration separation_req = separations_[sequence[j].get_class()][sequence[i].get_class()];
            time_duration separation = current_schedule_.get(sequence[i]) - current_schedule_.get(sequence[j]);
            if (separation < separation_req)
                total_cost = std::numeric_limits<double>::infinity();
        }
    }
    //print_schedule(schedule_result, sequence);
    return total_cost;
}

} //AMAN