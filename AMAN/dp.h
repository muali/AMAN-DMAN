#pragma once

#include "stdafx.h"
#include "data_types.h"
#include <bitset>

struct dp_solver
{
    dp_solver(size_t max_shift, uint64_t time_sampling);
    ~dp_solver();
    sequence solve(const input_data&);

private:

    struct state
    {
        std::vector<bool> mask;
        size_t last;
        uint64_t time;
        bool operator== (const state&) const;
    };
    
    struct state_hash
    {
        std::hash<std::vector<bool>> v_hash;
        state_hash();
        size_t operator()(const state&) const;
    };

    size_t max_shift_;
    uint64_t time_sampling_;
};