// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <fstream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>

using std::vector;
using std::priority_queue;
using std::shared_ptr;
using std::make_shared;

#include <boost\functional\hash.hpp>
#include <boost\date_time\posix_time\posix_time.hpp>

using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::posix_time::seconds;
using boost::posix_time::minutes;



// TODO: reference additional headers your program requires here
