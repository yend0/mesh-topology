#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdatomic.h>

atomic_int message_id = ATOMIC_VAR_INIT(0);

#endif // GLOBALS_H