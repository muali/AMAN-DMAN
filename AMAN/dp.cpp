#include "dp.h"
#include "sequencers.h"

dp_solver::dp_solver(size_t max_shift, uint64_t time_sampling)
    : max_shift_(max_shift)
    , time_sampling_(time_sampling)
{
}

dp_solver::~dp_solver()
{
}

sequence dp_solver::solve(const input_data& data)
{
    using namespace std;
    using state_r = pair < double, state > ;
    using state_q = pair < size_t, state > ;
    
    sequence by_order = fcfs(data);
    state initial_state{ vector<bool>(by_order.size()), numeric_limits<size_t>::max(), 0 };
    
    unordered_map<state, state_r, state_hash> dp;
    dp[initial_state] = make_pair(0., initial_state);

    queue<state_q> order;
    order.push(make_pair(0, initial_state));
    
    while (order.front().first != by_order.size())
    {
        state_q current = order.front();
        order.pop();
        size_t end = min(current.first + max_shift_ + 1, by_order.size());
        size_t force_landing = current.first - max_shift_;
        for (size_t i = max(0U, force_landing); i != end; ++i)
        {
            if (!current.second.mask[i])
            {
                state next{ current.second.mask, i, 0 };
                next.mask[i] = true;
                uint64_t min_time = max(current.first == 0 ? 0 : current.second.time + data.separations[current.second.last][i],
                    by_order[i].earleast_landing);
                uint64_t max_time = max(min_time, by_order[i].target_time + time_sampling_ - 1);
                max_time = min(max_time, by_order[i].latest_landing);
                for (uint64_t time = min_time; time <= max_time; time += time_sampling_)
                {
                    next.time = time;
                    double cost = dp[current.second].first + time <= by_order[i].target_time ?
                        (by_order[i].target_time - time) * by_order[i].cost_before_landing :
                        (time - by_order[i].target_time) * by_order[i].cost_after_landing;
                    if (dp.find(next) != dp.end())
                    {
                        if (dp[next].first > cost)
                            dp[next] = make_pair(cost, current.second);
                    }
                    else
                    {
                        dp[next] = make_pair(cost, current.second);
                        order.push(make_pair(current.first + 1, next));
                    }
                }
                if (i == force_landing)
                    break;
            }
        }
    }
    state best = initial_state;
    while (!order.empty())
    {
        state_q current = order.front;
        order.pop();
        if (best == initial_state || dp[best].first > dp[current.second].first)
            best = current.second;
    }
    if (best == initial_state)
        assert(false); //TODO: No solution
    sequence result;
    while (!(dp[best].second == initial_state))
    {
        result.push_back(by_order[best.last]);
        best = dp[best].second;
    }
    reverse(result.begin(), result.end());
    return result;
}

bool dp_solver::state::operator== (const dp_solver::state& other) const
{
    return mask == other.mask
        && time == other.time
        && last == other.last;
}

dp_solver::state_hash::state_hash()
    : v_hash()
{
}

size_t dp_solver::state_hash::operator() (const dp_solver::state& obj) const
{
    size_t seed = v_hash(obj.mask);
    boost::hash_combine(seed, obj.last);
    boost::hash_combine(seed, obj.time);
    return seed;
}
