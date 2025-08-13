#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"

#define RTC_TIME_LOW        0x00
#define RTC_TIME_HIGH       0x04
#define RTC_ALARM_LOW       0x08
#define RTC_ALARM_HIGH      0x0c
#define RTC_IRQ_ENABLED     0x10
#define RTC_CLEAR_ALARM     0x14
#define RTC_ALARM_STATUS    0x18
#define RTC_CLEAR_INTERRUPT 0x1c

uint64
rtcread(void)
{
  return *(volatile uint64 *)(RTC + RTC_TIME_LOW);
}
