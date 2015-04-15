#pragma once

#include "input_data.h"
#include "schedule.h"

#include <vector>
#include <memory>

namespace AMAN
{
    //-------------------------------------------------------------------------------------

    struct scheduler
    {
        virtual schedule do_scheduling(const input_data&, std::vector<aircraft> sequence) const = 0;
        virtual ~scheduler() = default;
    };

    //-------------------------------------------------------------------------------------

    struct estimator
    {
        estimator(std::shared_ptr<scheduler>);
        double estimate(const input_data&, std::vector<aircraft> sequence) const;
    private:
        std::shared_ptr<scheduler> p_scheduler_;
    };

    struct greedy_scheduler
        : scheduler
    {
        schedule do_scheduling(const input_data&, std::vector<aircraft> sequence) const override;
    };

    //-------------------------------------------------------------------------------------

    struct dp_scheduler
        : scheduler
    {
        dp_scheduler(uint64_t time_sampling);
        
        schedule do_scheduling(const input_data&, std::vector<aircraft> sequence) const override;
    };

    //-------------------------------------------------------------------------------------

    struct linear_scheduler
    {

    };

} // AMAN