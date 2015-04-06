#pragma once

#include "stdafx.h"

using id_type = size_t;

struct aircraft
{
    id_type id;
    uint64_t appearance_time;
    uint64_t earleast_landing;
    uint64_t latest_landing;
    uint64_t target_time;
    double cost_before_landing;
    double cost_after_landing;
};

struct input_data
{
    std::vector<aircraft> aircrafts;
    double freeze_time;
    std::vector<std::vector<uint64_t>> separations;
};

using sequence = std::vector < aircraft > ;
using schedule = std::vector < std::pair< id_type, double> > ;

using sequencer = sequence(*) (const input_data&);
using scheduler = schedule(*) (const input_data&, sequence);
using solver    = schedule(*) (const input_data&);
