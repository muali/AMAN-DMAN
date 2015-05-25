#include "stdafx.h"
#include "schedulers.h"

namespace AMAN
{

estimator::estimator(shared_ptr<scheduler> p_scheduler)
    : p_scheduler_(p_scheduler)
{

}

double estimator::estimate(const input_data& data, vector<aircraft> sequence) const
{
    schedule schedule_result = p_scheduler_->do_scheduling(data, sequence);
    double total_cost = 0.;
    for (size_t i = 0; i < sequence.size(); ++i)
    {
        total_cost += sequence[i].estimate_cost(schedule_result.get(sequence[i]));
        if (i > 0)
        {
            time_duration separation_req = data.get_separation(sequence[i - 1], sequence[i]);
            time_duration separation = schedule_result.get(sequence[i]) - schedule_result.get(sequence[i - 1]);
            if (separation < separation_req)
                total_cost = std::numeric_limits<double>::infinity();
        }
    }
    //print_schedule(schedule_result, sequence);
    return total_cost;
}


schedule greedy_scheduler::do_scheduling(const input_data& data, const vector<aircraft>& sequence) const
{
    using namespace std;
    using namespace boost::posix_time;
    schedule result;
    assert(sequence.size() > 0);
    ptime last_landing(data.get_start_time(sequence[0].get_class()));
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
        last_landing = ptime(std::max(sequence[i].get_min_time(), last_landing + separation));
        result.set(sequence[i], last_landing);
    }
    return result;
}

namespace
{

struct cost_change
{
    time_duration at_;
    double change_;
    cost_change(const time_duration& at, double change)
        : at_(at)
        , change_(change)
    {
    }
};

bool operator> (const cost_change& a, const cost_change& b)
{
    return a.at_ > b.at_;
}

using cost_change_pq = priority_queue < cost_change, vector<cost_change>, std::greater<cost_change> > ;

struct group
{       
    group() = default;
    group(const aircraft& item, const time_duration& separation_to_next, const ptime& min_time);
    group(const aircraft& item, shared_ptr<group> prev, const time_duration& separation_to_next, const ptime& min_time);

    shared_ptr<group> merge(shared_ptr<group> left, shared_ptr<group> right);

    bool optimize();
    void traverse(schedule& schedule, vector<aircraft>::const_iterator& sequence_it, time_duration shift) const;

private:
   
    ptime last_seated_;
    time_duration merge_time_;
    time_duration really_shift_;
    time_duration optimization_shift_;
    shared_ptr<cost_change_pq> changes_;
    shared_ptr<group> left_;
    shared_ptr<group> right_;

    double current_cost_;

};

group::group(const aircraft& item, const time_duration& separation_to_next, const ptime& min_time)
    : last_seated_(item.get_max_time() + separation_to_next)
    , merge_time_(boost::posix_time::pos_infin)
    , really_shift_()
    , optimization_shift_()
    , changes_(make_shared<cost_change_pq>())
    , left_()
    , right_()
    , current_cost_(-item.get_cost_per_second_after())
{
    changes_->push(cost_change(item.get_max_time() - item.get_target_time(),
        item.get_cost_per_second_after() + item.get_cost_per_second_before()));
    changes_->push(cost_change(item.get_max_time() - item.get_min_time(), std::numeric_limits<double>::infinity()));
    changes_->push(cost_change(item.get_max_time() - min_time, std::numeric_limits<double>::infinity()));
}

group::group(const aircraft& item, shared_ptr<group> prev, const time_duration& separation_to_next, const ptime& min_time)
    : group(item, separation_to_next, min_time)
{
    merge_time_ = last_seated_ - separation_to_next - prev->last_seated_;
}

shared_ptr<group> group::merge(shared_ptr<group> left, shared_ptr<group> right)
{
    shared_ptr<group> res = make_shared<group>();
    res->really_shift_ = time_duration();
    res->left_  = left;
    res->right_ = right;
    res->current_cost_ = current_cost_ + right->current_cost_;
    res->last_seated_ = right->last_seated_;
    if (changes_->size() < right->changes_->size())
    {
        res->optimization_shift_ = right->optimization_shift_;
        res->merge_time_ = merge_time_ - optimization_shift_ + right->optimization_shift_;
        res->changes_    = right->changes_;
        while (!changes_->empty())
        {
            cost_change change = changes_->top();
            changes_->pop();
            change.at_ = change.at_ - optimization_shift_ + right->optimization_shift_;
            res->changes_->push(change);
        }
    }
    else
    {
        res->optimization_shift_ = optimization_shift_;
        res->merge_time_ = merge_time_;
        res->changes_ = changes_;
        while (!right->changes_->empty())
        {
            cost_change change = right->changes_->top();
            right->changes_->pop();
            change.at_ = change.at_ - right->optimization_shift_ + optimization_shift_;
            res->changes_->push(change);
        }
    }
    return res;
}

bool group::optimize()
{
    while (current_cost_ < 0 && optimization_shift_ < merge_time_)
    {
        assert(!changes_->empty());
        time_duration dif = std::min(merge_time_, changes_->top().at_) - optimization_shift_;
        optimization_shift_ += dif;
        really_shift_ += dif;
        last_seated_ -= dif;
        while (!changes_->empty() && changes_->top().at_ == optimization_shift_)
        {
            current_cost_ += changes_->top().change_;
            changes_->pop();
        }
    }
    return optimization_shift_ == merge_time_;
}

void group::traverse(schedule& schedule, vector<aircraft>::const_iterator& sequence_it, time_duration shift) const
{
    if (!left_) //is leaf
    {
        schedule.set(*sequence_it, sequence_it->get_max_time() - shift - really_shift_);
        ++sequence_it;
    }
    else
    {
        shift += really_shift_;
        left_->traverse(schedule, sequence_it, shift);
        right_->traverse(schedule, sequence_it, shift);
    }
}

} //Unnamed

schedule linear_scheduler::do_scheduling(const input_data& data, const vector<aircraft>& sequence) const
{
    vector<shared_ptr<group> > groups;
    time_duration separation_to_next;
    if (sequence.size() > 1)
        separation_to_next = data.get_separation(sequence[0], sequence[1]);
    assert(sequence.size() != 0);
    ptime min_time = data.get_start_time(sequence[0].get_class());
    groups.push_back(make_shared<group>(*sequence.begin(), separation_to_next, min_time));
    groups[0]->optimize();
    auto seq_it = sequence.begin() + 1;
    while (seq_it != sequence.end())
    {
        if (seq_it + 1 != sequence.end())
            separation_to_next = data.get_separation(*seq_it, *(seq_it + 1));
        else
            separation_to_next = time_duration();

        shared_ptr<group> last = make_shared<group>(*seq_it, *groups.rbegin(), separation_to_next, min_time);
        while (last->optimize())
        {
            shared_ptr<group> prev = *groups.rbegin();
            groups.pop_back();
            last = prev->merge(prev, last);
        }
        ++seq_it;
        groups.push_back(last);
    }

    schedule result;
    seq_it = sequence.begin();
    for (auto item : groups)
    {
        item->traverse(result, seq_it, time_duration());
    }
    return result;
}

} //AMAN
