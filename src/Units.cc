#include "Units.h"


Units::Units()
  : DynamicLibrary("./units.so"),
    size(lookup<const unsigned>("num_units")),
    numBPreds(lookup<const int>("NumBPreds")),
    numFPreds(lookup<const int>("NumFPreds")),
    numGPreds(lookup<const int>("NumGPreds")),
    numRPreds(lookup<const int>("NumRPreds")),
    numSPreds(lookup<const int>("NumSPreds")),
    infos(lookup<const unit_t []>("units"))
{
}
