#include "Sites.h"


Sites::Sites()
  : DynamicLibrary("./sites.so"),
    size(lookup<const unsigned>("num_sites")),
    infos(lookup<const site_t []>("sites"))
{
}
