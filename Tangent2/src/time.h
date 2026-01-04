/*
 * time.h
 * Interface for time management functions
 *  Created on: Jan 5, 2026
 *      Author: harma
 */
 
#ifndef TIME_H_
#define TIME_H_

#include "base.h"
#define RTC_ENABLE *((volatile __near byte *)0xF0C7)
#define RTC_SECOND *((volatile __near byte *)0xF0C0)
#define RTC_MINUTE *((volatile __near byte *)0xF0C1)
#define RTC_HOUR *((volatile __near byte *)0xF0C2)
#define RTC_DAY *((volatile __near byte *)0xF0C3) // Note: ML620909 RTC clock does not use crystal oscillator.
#define RTC_WEEK *((volatile __near byte *)0xF0C4) // Therefore, it may drift significantly over time (~2 minutes per hour)
#define RTC_MONTH *((volatile __near byte *)0xF0C5) // It also depends on certain core configurations and battery voltage, 
#define RTC_YEAR *((volatile __near byte *)0xF0C6) // so it is advised to not use the RTC for precise timekeeping.

#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define DAYS_PER_WEEK 7
#define MONTHS_PER_YEAR 12

#define TICKS_PER_MS 8
#define TICKS_PER_SECOND 8000

// TODO: alarms

void rtc_reset();
void rtc_enable();
void rtc_disable();

void rtc_set_time(byte hour, byte minute, byte second);
void rtc_set_seconds(byte second);
void rtc_set_minutes(byte minute);
void rtc_set_hours(byte hour);

void rtc_get_time(byte* hour, byte* minute, byte* second);
byte rtc_get_seconds();
byte rtc_get_minutes();
byte rtc_get_hours();

void rtc_set_date(word year, byte month, byte day, byte week);
void rtc_set_day(byte day);
void rtc_set_month(byte month);
void rtc_set_year(word year);

void rtc_get_date(word* year, byte* month, byte* day, byte* week);
byte rtc_get_day();
byte rtc_get_month();
word rtc_get_year();

void delay_ms(word ms);
void delay_s(word s);

void ms_to_ticks(word ms, word* ticks);
void s_to_ticks(word s, word* ticks);
word ticks_to_ms(word ticks);
word ticks_to_s(word ticks);

#endif /* TIME_H_ */