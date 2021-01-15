/*
 * Copyright (C) 2010, 2013-2014, 2018 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_osk_time.c
 * Implementation of the OS abstraction layer for the kernel device driver
 */

#include "mali_osk.h"
#include <linux/jiffies.h>
#include <linux/time.h>
#include <asm/delay.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
struct timespec {
  __kernel_old_time_t     tv_sec;         /* seconds */
  long                    tv_nsec;        /* nanoseconds */
};
#if __BITS_PER_LONG == 64
/* timespec64 is defined as timespec here */
static inline struct timespec timespec64_to_timespec(const struct timespec64 ts64)
{
  return *(const struct timespec *)&ts64;
}

static inline struct timespec64 timespec_to_timespec64(const struct timespec ts)
{
  return *(const struct timespec64 *)&ts;
}
#else
static inline struct timespec timespec64_to_timespec(const struct timespec64 ts64)
{
  struct timespec ret;

  ret.tv_sec = ts64.tv_sec;
  ret.tv_nsec = ts64.tv_nsec;
  return ret;
}

static inline struct timespec64 timespec_to_timespec64(const struct timespec ts)
{
  struct timespec64 ret;

  ret.tv_sec = ts.tv_sec;
  ret.tv_nsec = ts.tv_nsec;
  return ret;
}
#endif
static inline s64 timespec_to_ns(const struct timespec *ts)
{
  return ((s64) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}
static inline void getnstimeofday(struct timespec *ts)
{
  struct timespec64 ts64;

  ktime_get_real_ts64(&ts64);
  *ts = timespec64_to_timespec(ts64);
}
#endif /*LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)*/

mali_bool _mali_osk_time_after_eq(unsigned long ticka, unsigned long tickb)
{
	return time_after_eq(ticka, tickb) ?
	       MALI_TRUE : MALI_FALSE;
}

unsigned long _mali_osk_time_mstoticks(u32 ms)
{
	return msecs_to_jiffies(ms);
}

u32 _mali_osk_time_tickstoms(unsigned long ticks)
{
	return jiffies_to_msecs(ticks);
}

unsigned long _mali_osk_time_tickcount(void)
{
	return jiffies;
}

void _mali_osk_time_ubusydelay(u32 usecs)
{
	udelay(usecs);
}

u64 _mali_osk_time_get_ns(void)
{
	struct timespec tsval;
	getnstimeofday(&tsval);
	return (u64)timespec_to_ns(&tsval);
}

u64 _mali_osk_boot_time_get_ns(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0)
	return ktime_get_boottime_ns();
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0)
	return ktime_get_boot_ns();
#else
	struct timespec tsval;
	get_monotonic_boottime(&tsval);
	return (u64)timespec_to_ns(&tsval);
#endif
}
