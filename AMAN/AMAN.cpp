// AMAN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "schedulers.h"
#include "or_lib_data_provider.h"
#include "sa_sequencer.h"
#include "dp.h"

using namespace AMAN;
using namespace std;


void schedulers_test()
{
    shared_ptr<scheduler> greedy = make_shared<greedy_scheduler>();
    shared_ptr<scheduler> linear = make_shared<linear_scheduler>();
    or_lib_data_provider data_source("dataset\\");
    estimator est_greedy(greedy);
    estimator est_linear(linear);
    while (data_source.has_next())
    {
        test_data data2 = data_source.read_next();
        input_data data = data2.next();
        vector<aircraft> seq = data.get_unordered();
        cout << "Dataset: " << seq.size() << endl;
        clock_t start = clock();
        int iter = 0;
        for (; clock() - start < CLOCKS_PER_SEC;)
        {
            //double greedy_res = est_greedy.estimate(data, seq);
            //cout << "Greedy: " << greedy_res << endl;
            linear->do_scheduling(data, seq);
               // est_linear.estimate(data, seq);
            //cout << "Linear: " << linear_res << endl;
            ++iter;
            std::random_shuffle(seq.begin(), seq.end());
        }

        cout << "Iter: " << iter << endl;
        cout << "Avg: " << 1e6 / iter << endl;
    }
}

void sequencers_test()
{
    or_lib_data_provider data_source("dataset\\");
    linear_scheduler scheduler;
    sa_sequencer<linear_scheduler> sequencer(50., 1e-4, 0.99);
    //fcfs_sequencer sequencer;
    uint32_t dataset = 1;
    while (data_source.has_next())
    {
        test_data data = data_source.read_next();
        double start = clock();
        pair<schedule, double> result = perform_test(data, sequencer, scheduler);
        double time = (clock() - start) / CLOCKS_PER_SEC;
        cout << "Dataset: " << dataset << endl;
        cout << "Cost: " << result.second << endl;
        cout << "Time: " << time << endl;
        ++dataset;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
   // schedulers_test();
    sequencers_test();
	return 0;
}

