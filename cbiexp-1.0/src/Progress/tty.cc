#include <iostream>
#include <cstdlib>
#include "../config.h"
#include "tty.h"

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#  define TTY_NAME ttyname(STDIN_FILENO)
#else // Windows
#  define TTY_NAME "con:"
#endif // Windows


std::ofstream Progress::tty(TTY_NAME);
