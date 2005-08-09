#include "Units.h"


Units::Units()
  : DynamicLibrary("./units.so"),
    size(lookup<const unsigned>("num_units")),
    numBPreds(lookup<const int>("NumBPreds")),
    numRPreds(lookup<const int>("NumRPreds")),
    numSPreds(lookup<const int>("NumSPreds")),
    numGPreds(lookup<const int>("NumGPreds")),
    infos(lookup<const unit_t []>("units"))
{
}
