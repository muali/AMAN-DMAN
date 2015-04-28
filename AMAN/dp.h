#pragma once

#include "stdafx.h"

#include "aircraft.h"
#include "input_data.h"

#include <vector>

#include <boost\date_time\posix_time\posix_time.hpp>

namespace AMAN
{
    struct dp_sequencer_small;

    struct dp_sequencer
    {
        dp_sequencer(size_t max_shift, boost::posix_time::time_duration time_sampling);
        std::vector<aircraft> build_sequence(const input_data& data) const;

    private:
        shared_ptr<dp_sequencer_small> small_;
    };
}
