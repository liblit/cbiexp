#include <cstdlib>
#include <unistd.h>
#include "enabled.h"


const bool Progress::enabled = isatty(STDOUT_FILENO) && getenv("TERM");
