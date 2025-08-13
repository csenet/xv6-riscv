#include "platform.h"

#include "util.h"

void
netinit(void)
{
    char msg[] = "Hello, SecCamp2025!";

    debugf("%s", msg);
    debugdump(msg, sizeof(msg));
}
