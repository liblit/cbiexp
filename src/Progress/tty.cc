#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "tty.h"


std::ofstream Progress::tty(ttyname(STDIN_FILENO));
