#pragma once

#include "stdafx.h"
#include "data_types.h"

struct or_lib_data_provider
{
    or_lib_data_provider(const std::string& path);
    ~or_lib_data_provider();

    input_data read_next();
    bool has_next();

private:
    std::string path_;
    size_t current_test_;
};

