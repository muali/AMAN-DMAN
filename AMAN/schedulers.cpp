#include "schedulers.h"

#include <algorithm>

#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{
    estimator::estimator(std::shared_ptr<scheduler> p_scheduler)
        : p_scheduler_(p_scheduler)
    {

    }

    double estimator::estimate(const input_data& data, std::vector<aircraft> sequence) const
    {
        schedule schedule_result = p_scheduler_->do_scheduling(data, sequence);
        double total_cost = 0.;
        for (aircraft& item : sequence)
        {
            total_cost += item.estimate_cost(schedule_result.get(item));
        }
        return total_cost;
    }

    schedule greedy_scheduler::do_scheduling(const input_data& data, std::vector<aircraft> sequence) const
    {
        using namespace std;
        using namespace boost::posix_time;
        schedule result;
        ptime last_landing(data.get_start_time());
        for (size_t i = 0; i < sequence.size(); ++i)
        {
            time_duration separation;
            if (i == 0)
            {
                separation = seconds(0);
            }
            else
            {
                separation = data.get_separation(sequence[i - 1], sequence[i]);
            }
            ptime landing_time(std::max(sequence[i].get_min_time(), last_landing + separation));
            result.set(sequence[i], landing_time);
        }
        return result;
    }
}
