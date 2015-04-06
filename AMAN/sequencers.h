#pragma once

#include "data_types.h"

inline sequence fcfs(const input_data& data)
{
    sequence seq = data.aircrafts;
    sort(seq.begin(), seq.end(), 
        [](aircraft a, aircraft b)
    {
        return a.appearance_time < b.appearance_time;
    });
    return seq;
}