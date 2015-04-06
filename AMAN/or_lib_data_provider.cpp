#include "stdafx.h"
#include "or_lib_data_provider.h"

namespace
{
    const size_t tests_count = 13;
    std::string prefix = "airland";
}

or_lib_data_provider::or_lib_data_provider(const std::string& path)
    : path_(path)
    , current_test_(1)
{
}

or_lib_data_provider::~or_lib_data_provider()
{
}

input_data or_lib_data_provider::read_next()
{
    assert(current_test_ <= tests_count);
    using namespace std;
    string path_to_test = path_ + prefix + to_string(current_test_);
    std::ifstream istream(path_to_test);
    
    input_data test;

    size_t count;
    istream >> count;
    int32_t unused_data;
    istream >> test.freeze_time;

    test.aircrafts.resize(count);
    test.separations.resize(count);
    for (size_t i = 0; i < count; ++i)
    {
        aircraft& current = test.aircrafts[i];
        current.id = i;

        istream >> current.appearance_time
            >> current.earleast_landing
            >> current.target_time
            >> current.latest_landing
            >> current.cost_before_landing
            >> current.cost_after_landing;

        test.separations[i].resize(count);
        for (size_t j = 0; j < count; ++j)
        {
            istream >> test.separations[i][j];
        }
    }
    ++current_test_;
    return test;
}

bool or_lib_data_provider::has_next()
{
    return current_test_ <= tests_count;
}