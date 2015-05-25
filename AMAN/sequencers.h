#pragma once

#include "aircraft.h"
#include "input_data.h"

#include <algorithm>
#include <vector>

namespace AMAN
{

inline std::vector<aircraft> fcfs(const input_data& data)
{
    std::vector<aircraft> seq = data.get_unordered();
    std::sort(seq.begin(), seq.end(),
        [](aircraft a, aircraft b)
    {
        return a.get_min_time() < b.get_min_time();
    });
    return seq;
}

}
