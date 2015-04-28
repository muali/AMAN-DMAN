#include "stdafx.h"
#include "dp.h"
#include "sequencers.h"
#include "input_data.h"
#include <memory>
#include <vector>
#include <queue>
#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{
    struct small_dp_state
    {
        uint64_t mask;
        uint8_t last_class;
        std::shared_ptr<boost::posix_time::ptime> last_landing;
    };
}

namespace std
{
    template <>
    struct hash < AMAN::small_dp_state >
    {
        size_t operator() (const AMAN::small_dp_state& key_val) const
        {
            size_t seed = static_cast<size_t>(key_val.mask);
            boost::hash_combine(seed, key_val.last_class);
            boost::hash_combine(seed, key_val.last_landing->time_of_day().seconds());
            return seed;
        }
    };
}

namespace AMAN
{
    struct dp_sequencer_small
    {
        dp_sequencer_small(size_t max_shift, boost::posix_time::time_duration time_sampling);
        std::vector<aircraft> build_sequence(const input_data& data) const;
    private:
        size_t max_shift_;
        boost::posix_time::time_duration time_sampling_;
    };

    bool operator==(const small_dp_state& lhs, const small_dp_state& rhs)
    {
        return lhs.mask == rhs.mask &&
            lhs.last_class == rhs.last_class &&
            *lhs.last_landing == *rhs.last_landing;
    }

    dp_sequencer_small::dp_sequencer_small(size_t max_shift, boost::posix_time::time_duration time_sampling)
        : max_shift_(max_shift)
        , time_sampling_(time_sampling)
    {
    }

    std::vector<aircraft> dp_sequencer_small::build_sequence(const input_data& data) const
    {
        using namespace std;
        using namespace boost::posix_time;
        using state = small_dp_state;
        //using state_r = pair < double, state > ;
        using state_r = struct
        {
            double cost;
            state st;
            uint32_t last;
        };
        using state_q = pair < size_t, state > ;

        vector<aircraft> ordered = fcfs(data);

        state initial_state{ 0, numeric_limits<uint8_t>::max(), make_shared<ptime>(data.get_start_time()) };

        unordered_map<state, state_r> dp;
        dp[initial_state] = state_r{ 0., initial_state, -1 };

        queue<state_q> order;
        order.push(make_pair(0, initial_state));

        while (order.front().first != ordered.size())
        {
            state_q current = order.front();
            order.pop();
            size_t end = min(current.first + max_shift_ + 1, ordered.size());
            size_t force_landing = current.first - max_shift_;
            for (size_t i = max(0U, force_landing); i != end; ++i)
            {
                if (current.second.mask && (1ULL << i))
                    continue;

                ptime min_time = data.get_start_time();
                if (current.first != 0)
                {
                    min_time = *current.second.last_landing + data.get_separation(current.second.last_class, ordered[i].get_class());
                }
                min_time = min(min_time, ordered[i].get_min_time());
                
                ptime max_time = max(min_time + seconds(1), ordered[i].get_target_time() + time_sampling_);
                max_time = min(max_time, ordered[i].get_max_time() + seconds(1));
                
                for (ptime time = min_time; time < max_time; time += time_sampling_)
                {
                    state next{ (current.second.mask | (1ULL << i)), ordered[i].get_class(), make_shared<ptime>(time) };
                    double cost = dp[current.second].cost + ordered[i].estimate_cost(time);
                    if (dp.find(next) != dp.end())
                    {
                        if (dp[next].cost > cost)
                            dp[next] = state_r{ cost, current.second, i };
                    }
                    else
                    {
                        dp[next] = state_r{ cost, current.second, i };
                        order.push(make_pair(current.first + 1, next));
                    }
                }

                if (i == force_landing)
                    break;
            }
        }

        state best = initial_state;
        while (!order.empty())
        {
            state_q current = order.front();
            order.pop();
            if (best == initial_state || dp[best].cost > dp[current.second].cost)
                best = current.second;
        }
        if (best == initial_state)
            assert(false); //TODO: No solution
        vector<aircraft> result;
        while (!(dp[best].st == initial_state))
        {
            result.push_back(ordered[dp[best].last]);
            best = dp[best].st;
        }
        reverse(result.begin(), result.end());
        return result;
    }

    dp_sequencer::dp_sequencer(size_t max_shift, boost::posix_time::time_duration time_sampling)
        : small_(make_shared<dp_sequencer_small>(max_shift, time_sampling))
    {
    }

    std::vector<aircraft> dp_sequencer::build_sequence(const input_data& data) const
    {
        if (data.get_unordered().size() <= 64)
            return small_->build_sequence(data);
        //TODO: implement dp_sequencer_large
        assert(false);
    }
}



//dp_solver::dp_solver(size_t max_shift, uint64_t time_sampling)
//    : max_shift_(max_shift)
//    , time_sampling_(time_sampling)
//{
//}
//
//dp_solver::~dp_solver()
//{
//}
//
//sequence dp_solver::solve(const input_data& data)
//{
//    using namespace std;
//    using state_r = pair < double, state > ;
//    using state_q = pair < size_t, state > ;
//    
//    sequence by_order = fcfs(data);
//    state initial_state{ vector<bool>(by_order.size()), numeric_limits<size_t>::max(), 0 };
//    
//    unordered_map<state, state_r, state_hash> dp;
//    dp[initial_state] = make_pair(0., initial_state);
//
//    queue<state_q> order;
//    order.push(make_pair(0, initial_state));
//    
//    while (order.front().first != by_order.size())
//    {
//        state_q current = order.front();
//        order.pop();
//        size_t end = min(current.first + max_shift_ + 1, by_order.size());
//        size_t force_landing = current.first - max_shift_;
//        for (size_t i = max(0U, force_landing); i != end; ++i)
//        {
//            if (!current.second.mask[i])
//            {
//                state next{ current.second.mask, i, 0 };
//                next.mask[i] = true;
//                uint64_t min_time = max(current.first == 0 ? 0 : current.second.time + data.separations[current.second.last][i],
//                    by_order[i].earleast_landing);
//                uint64_t max_time = max(min_time, by_order[i].target_time + time_sampling_ - 1);
//                max_time = min(max_time, by_order[i].latest_landing);
//                for (uint64_t time = min_time; time <= max_time; time += time_sampling_)
//                {
//                    next.time = time;
//                    double cost = dp[current.second].first + time <= by_order[i].target_time ?
//                        (by_order[i].target_time - time) * by_order[i].cost_before_landing :
//                        (time - by_order[i].target_time) * by_order[i].cost_after_landing;
//                    if (dp.find(next) != dp.end())
//                    {
//                        if (dp[next].first > cost)
//                            dp[next] = make_pair(cost, current.second);
//                    }
//                    else
//                    {
//                        dp[next] = make_pair(cost, current.second);
//                        order.push(make_pair(current.first + 1, next));
//                    }
//                }
//                if (i == force_landing)
//                    break;
//            }
//        }
//    }
//    state best = initial_state;
//    while (!order.empty())
//    {
//        state_q current = order.front;
//        order.pop();
//        if (best == initial_state || dp[best].first > dp[current.second].first)
//            best = current.second;
//    }
//    if (best == initial_state)
//        assert(false); //TODO: No solution
//    sequence result;
//    while (!(dp[best].second == initial_state))
//    {
//        result.push_back(by_order[best.last]);
//        best = dp[best].second;
//    }
//    reverse(result.begin(), result.end());
//    return result;
//}
//
//bool dp_solver::state::operator== (const dp_solver::state& other) const
//{
//    return mask == other.mask
//        && time == other.time
//        && last == other.last;
//}
//
//dp_solver::state_hash::state_hash()
//    : v_hash()
//{
//}
//
//size_t dp_solver::state_hash::operator() (const dp_solver::state& obj) const
//{
//    size_t seed = v_hash(obj.mask);
//    boost::hash_combine(seed, obj.last);
//    boost::hash_combine(seed, obj.time);
//    return seed;
//}
