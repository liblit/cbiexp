#ifndef INCLUDE_RunType_h
#define INCLUDE_RunType_h

#include "../arguments.h"

namespace RunType
{
    extern const argp argp;
    extern unsigned int type;
    enum { FAILING_RUNS, SUCCEEDING_RUNS };
}

#endif // !INCLUDE_RunType_h
