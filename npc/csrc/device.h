#ifndef __DEVICE_H__
#define __DEVICE_H__


#define SERIAL_ADDR    0xa00003f8
#define RTC_ADDR       0xa0000048

#include <sys/time.h>
#include <stdint.h>
static struct timeval boot_time;
static int initialized = 0;

uint32_t get_uptime_32bit() {
  struct timeval now;
  gettimeofday(&now, NULL);

  if (!initialized) {
    boot_time = now;
    initialized = 1;
  }

  uint64_t now_us = now.tv_sec * 1000000ull + now.tv_usec;
  uint64_t boot_us = boot_time.tv_sec * 1000000ull + boot_time.tv_usec;
  uint64_t uptime_us = now_us - boot_us;

  return (uint32_t)uptime_us;  // 低 32 位
}

#endif

