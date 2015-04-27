#include "schedulers.h"
#include "stdafx.h"

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

schedule greedy_scheduler::do_scheduling(const input_data& data, const std::vector<aircraft>& sequence) const
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
    group(const aircraft& item);
    group(const aircraft& item, shared_ptr<group> prev);

    shared_ptr<group> merge(shared_ptr<group> right);

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

group::group(const aircraft& item)
    : last_seated_(item.get_max_time())
    , merge_time_(boost::posix_time::neg_infin)
    , really_shift_()
    , optimization_shift_()
    , changes_(make_shared<cost_change_pq>())
    , left_()
    , right_()
    , current_cost_(-item.get_cost_per_second_after())
{
    changes_->push(cost_change(last_seated_ - item.get_target_time(),
        item.get_cost_per_second_after() + item.get_cost_per_second_before()));
    changes_->push(cost_change(last_seated_ - item.get_min_time(), std::numeric_limits<double>::infinity()));
}

group::group(const aircraft& item, shared_ptr<group> prev)
    : group(item)
{
    merge_time_ = last_seated_ - prev->last_seated_;
}

shared_ptr<group> group::merge(shared_ptr<group> right)
{
    shared_ptr<group> res = make_shared<group>();
    res->really_shift_ = time_duration();
    res->left_  = shared_ptr<group>(this);
    res->right_ = right;
    res->current_cost_ = current_cost_ + right->current_cost_;
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
        while (!right_->changes_->empty())
        {
            cost_change change = right->changes_->top();
            right->changes_->pop();
            change.at_ = change.at_ - right->optimization_shift_ + optimization_shift_;
            res->changes_->push(change);
        }
    }
}

bool group::optimize()
{
    while (current_cost_ < 0 && optimization_shift_ < merge_time_)
    {
        assert(!changes_->empty());
        time_duration dif = std::min(merge_time_, changes_->top().at_) - optimization_shift_;
        optimization_shift_ += dif;
        really_shift_ += dif;
        while (changes_->top().at_ == optimization_shift_)
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
    groups.push_back(make_shared<group>(*sequence.begin()));
    auto seq_it = sequence.begin() + 1;
    while (seq_it != sequence.end())
    {
        shared_ptr<group> last = make_shared<group>(*seq_it, *groups.rbegin());
        while (last->optimize())
        {
            shared_ptr<group> prev = *groups.rbegin();
            groups.pop_back();
            last = prev->merge(last);
        }
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
