#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "time.h"

time_t
time(time_t *t)
{
  time_t _t;
  if (!t)
    t = &_t;
  *t = rtcread() / 1000000000;
  return *t;
}

int
gettimeofday(struct timeval *tv, void *tz)
{
  (void)tz;
  uint64 rtc = rtcread();
  tv->tv_sec = rtc / 1000000000;
  tv->tv_usec = (rtc % 1000000000) / 1000;
  return 0;
}
