#pragma once

#include "data_types.h"

//-------------------------------------------------------------------------------------

struct scheduler
{
    virtual schedule do_scheduling(const input_data&, sequence) const = 0;
    virtual ~scheduler() = default;
};

//-------------------------------------------------------------------------------------

struct greedy_scheduler
    :   scheduler
{
    greedy_scheduler() = default;
    ~greedy_scheduler() = default;

    schedule do_scheduling(const input_data&, sequence) const override;
};

//-------------------------------------------------------------------------------------

struct dp_scheduler
    :   scheduler
{
    dp_scheduler(uint64_t time_sampling);
    ~dp_scheduler();

    schedule do_scheduling(const input_data&, sequence) const override;
};

//-------------------------------------------------------------------------------------

struct linear_scheduler
{

};