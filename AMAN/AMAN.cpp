// AMAN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "schedulers.h"
#include "or_lib_data_provider.h"

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
        input_data data = data_source.read_next();
        vector<aircraft> seq = data.get_unordered();
        cout << "Dataset: " << seq.size() << endl;
        for (int i = 0; i < 10000; ++i)
        {
            double greedy_res = est_greedy.estimate(data, seq);
            //cout << "Greedy: " << greedy_res << endl;
            double linear_res = est_linear.estimate(data, seq);
            //cout << "Linear: " << linear_res << endl;
            
            assert((greedy_res == numeric_limits<double>::infinity()) == (linear_res == numeric_limits<double>::infinity()));
            assert(greedy_res >= linear_res);

            std::random_shuffle(seq.begin(), seq.end());
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    schedulers_test();
	return 0;
}

