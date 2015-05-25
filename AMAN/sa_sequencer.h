#pragma once

#include <cmath>
#include <chrono>
#include <memory>
#include <vector>
#include <random>

#include "aircraft.h"
#include "input_data.h"
#include "schedulers.h"
#include "sequencers.h"

namespace AMAN
{

template <class Scheduler, class Estimator = estimator>
struct sa_sequencer
{
    sa_sequencer(double initial_temperature = 10000.,
                 double minimum_temperature = 0.005,
                 double temperature_factor = 0.95);
    std::vector<aircraft> build_sequence(const input_data& data);

private:
    double initial_temp_;
    double min_temp_;
    double temp_factor_;
    std::minstd_rand rand_generator_;
    std::uniform_int_distribution<uint32_t> int_distr_;
    std::uniform_real_distribution<double> double_distr_;

    double temp_decrease(double current_temperature);
    bool accept(double best_cost, double cost, double temp);
    std::vector<aircraft> update(const std::vector<aircraft>& state);
};




template <class Scheduler, class Estimator>
sa_sequencer<Scheduler, Estimator>::sa_sequencer(double initial_temperature, 
                                                 double minimum_temperature,
                                                 double temperature_factor)
    : initial_temp_(initial_temperature)
    , min_temp_(minimum_temperature)
    , temp_factor_(temperature_factor)
    , rand_generator_()
    , int_distr_()
    , double_distr_()
{
}

template <class Scheduler, class Estimator>
std::vector<aircraft> sa_sequencer<Scheduler, Estimator>::build_sequence(const input_data& data)
{
    using namespace std;
    uint32_t seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    rand_generator_.seed(seed);
    using clock = chrono::high_resolution_clock;
    clock::time_point begin = clock::now();

    Estimator estimator = Estimator(make_shared<Scheduler>());

    vector<aircraft> best = fcfs(data);
    double best_cost = estimator.estimate(data, best);
    vector<aircraft> current = best;
    double current_cost = best_cost;

    double temperature = initial_temp_;
    clock::duration td = clock::now() - begin;
    int_distr_ = uniform_int_distribution<uint32_t>(0, best.size() - 1);

    while (temperature > min_temp_)
    {
        vector<aircraft> next = update(current);
        double cost = estimator.estimate(data, next);
        if (cost < best_cost)
        {
            best = next;
            best_cost = cost;
        }
        if (accept(current_cost, cost, temperature))
        {
            current = next;
            current_cost = cost;
        }
        temperature = temp_decrease(temperature);
    }

    return best;
}

template <class Scheduler, class Estimator>
double sa_sequencer<Scheduler, Estimator>::temp_decrease(double current_temperature)
{
    return current_temperature * temp_factor_;
}

template <class Scheduler, class Estimator>
bool sa_sequencer<Scheduler, Estimator>::accept(double prev_cost, double cost, double temp)
{
    if (cost < prev_cost)
        return true;
    double accept_prob = exp((prev_cost - cost) / temp);
    double rnd = double_distr_(rand_generator_);
    return rnd < accept_prob;
}

template <class Scheduler, class Estimator>
std::vector<aircraft> sa_sequencer<Scheduler, Estimator>::update(const std::vector<aircraft>& state)
{
    std::vector<aircraft> result = state;
    uint32_t fst = int_distr_(rand_generator_);
    uint32_t snd = int_distr_(rand_generator_);
    std::swap(result[fst], result[snd]);
    return result;
}


} //AMAN