#ifndef INCLUDE_SampleRatesFile_h
#define INCLUDE_SampleRatesFile_h

#include <vector>
#include"arguments.h"

namespace SampleRatesFile
{
    extern const argp argp;
    extern const char* filename;
    extern std::vector<double> rates;
    extern void read_rates(); 
}


#endif //!INCLUDE_SampleRatesFile_h
