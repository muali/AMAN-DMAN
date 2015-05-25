#include "stdafx.h"

#include <chrono>
#include <random>

#include "sa_sequencer.h"

namespace AMAN
{


sa_sequencer::sa_sequencer(const scheduler& scheduler, const estimator& estimator)
    : scheduler_(scheduler)
    , estimator_(estimator)
{
}

vector<aircraft> sa_sequencer::build_sequence(const input_data& data)
{
    using namespace std;
    using clock = chrono::high_resolution_clock;
    clock::time_point begin = clock::now();

    minstd_rand generator;
    generator.seed();

    vector<aircraft> result = data.get_unordered();


}

}//