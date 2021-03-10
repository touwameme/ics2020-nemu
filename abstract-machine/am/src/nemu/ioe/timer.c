#include <am.h>
#include <nemu.h>
#include<stdio.h>
static uint64_t boot_time = 0;

void __am_timer_init() {
  boot_time = inl(RTC_ADDR+4)*1000000+((uint64_t)(inl(RTC_ADDR)));
}


void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  //printf("%d\n",(uint64_t)inl(RTC_ADDR)+((uint64_t)(inl(RTC_ADDR))<<32));
  uptime->us =inl(RTC_ADDR+4)*1000000+((uint64_t)(inl(RTC_ADDR)))+500-boot_time; 
  //printf("boot %d\n",boot_time);
  //printf("time is %d\n",uptime->us+boot_time);
}


void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
