#pragma once

#include <string>
#include "input_data.h"

namespace AMAN
{
    struct or_lib_data_provider
    {
        or_lib_data_provider(const std::string& path);
        ~or_lib_data_provider();

        input_data read_next();
        bool has_next();

    private:
        std::string path_;
        size_t current_test_;
        const size_t tests_count_ = 13;
        const std::string prefix_ = "airland";
    };
}



