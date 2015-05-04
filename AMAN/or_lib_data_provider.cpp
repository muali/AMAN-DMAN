#include "stdafx.h"
#include "or_lib_data_provider.h"
#include "aircraft.h"


namespace AMAN
{
    using std::string;

    vector<uint8_t> turbulence_class_assignment(const string& path, vector<vector<uint64_t>>& separations);

    or_lib_data_provider::or_lib_data_provider(const string& path)
        : path_(path)
        , current_test_(1)
    {
    }

    or_lib_data_provider::~or_lib_data_provider()
    {
    }

    input_data or_lib_data_provider::read_next()
    {
        assert(current_test_ <= tests_count_);
        using namespace std;
        using namespace boost::posix_time;
        string path_to_test = path_ + prefix_ + to_string(current_test_) + ".txt";
        ptime now = second_clock::local_time();

        vector<vector<uint64_t>> separations;
        vector<uint8_t> class_assignment = turbulence_class_assignment(path_to_test, separations);
        uint8_t class_count = separations.size();

        ifstream istream(path_to_test);

        size_t count;
        uint32_t freeze_time;
        istream >> count >> freeze_time;

        input_data test(now + minutes(freeze_time), class_count);

        for (size_t i = 0; i < count; ++i)
        {
            uint32_t appearance_time, min_time, target_time, max_time;
            istream >> appearance_time >> min_time >> target_time >> max_time;
            double cost_before_target, cost_after_target;
            istream >> cost_before_target >> cost_after_target;

            uint32_t unused;
            for (size_t j = 0; j < count; ++j)
            {
                istream >> unused;
            }

            test.add_aircraft(aircraft(
                aircraft_id::from_idx(i),
                class_assignment[i],
                now + minutes(min_time),
                now + minutes(max_time),
                now + minutes(appearance_time),
                now + minutes(target_time),
                cost_before_target,
                cost_after_target
                ));
        }

        for (uint8_t i = 0; i < class_count; ++i)
        {
            for (uint8_t j = 0; j < class_count; ++j)
            {
                test.set_separation(i, j, minutes(separations[i][j]));
            }
        }

        ++current_test_;
        return test;
    }

    bool or_lib_data_provider::has_next()
    {
        return current_test_ <= tests_count_;
    }

    vector<uint8_t> turbulence_class_assignment(const string& path, vector<vector<uint64_t>>& separations)
    {
        using namespace std;
        ifstream istream(path);
        double unused;
        size_t count;
        istream >> count >> unused;
        vector<vector<uint64_t>> raw_separations(count, vector<uint64_t>(count));
        for (auto& line: raw_separations)
        {
            for (size_t j = 0; j < 6; ++j)
            {
                istream >> unused;
            }
            for (uint64_t& separation : line)
            {
                istream >> separation;
            }
        }

        vector<bool> used(count);
        vector<uint8_t> class_assignment(count);
        uint8_t next_class = 0;
        for (size_t i = 0; i < count; ++i)
        {
            if (used[i])
                continue;
            used[i] = true;
            class_assignment[i] = next_class;
            for (size_t j = i + 1; j < count; ++j)
            {
                if (used[j])
                    continue;
                bool same_class = true;
                for (size_t k = 0; k < count; ++k)
                {
                    if (k == j || k == i)
                        continue;
                    if (   raw_separations[i][k] != raw_separations[j][k]
                        || raw_separations[k][i] != raw_separations[k][j])
                        same_class = false;
                }
                same_class = same_class && raw_separations[i][j] == raw_separations[j][i];
                if (same_class)
                {
                    used[j] = true;
                    class_assignment[j] = next_class;
                }
            }
            next_class++;
        }

        separations = vector<vector<uint64_t>>(next_class, vector<uint64_t>(next_class));
        for (size_t i = 0; i < count; ++i)
        {
            for (size_t j = 0; j < count; ++j)
            {
                if (i == j)
                    continue;
                separations[class_assignment[i]][class_assignment[j]] = raw_separations[i][j];
            }
        }
        
        return class_assignment;
    }
}

