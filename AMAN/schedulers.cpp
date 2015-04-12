#include "schedulers.h"

//-------------------------------------------------------------------------------------

schedule greedy_scheduler::do_scheduling(const input_data& data, sequence seq) const
{
    schedule result;
    uint64_t last_landing = 0;
    for (aircraft& current : seq)
    {
        uint64_t separation;
        if (result.empty())
        {
            separation = 0;
        }
        else
        {
            id_type last_id = result.rbegin()->first;
            separation = data.separations[last_id][current.id];
        }
        uint64_t landing_time = std::max(current.earleast_landing, last_landing + separation);
        result.push_back(std::make_pair(current.id, landing_time));
    }                                                              
    return result;
}