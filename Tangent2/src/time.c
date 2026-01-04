/*
 * time.c
 * Implementation of time management functions
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#include "time.h"

// RTC Control Functions

void rtc_reset() {
    RTC_ENABLE = 0;
    RTC_SECONDS = 0;
    RTC_MINUTES = 0;
    RTC_HOURS = 0;
    RTC_DAY = 1;
    RTC_WEEK = 1;
    RTC_MONTH = 1;
    RTC_YEAR = 0; // Year is done by adding "current" year to this value, in this case it will be: 2026
}

void rtc_enable() {
    RTC_ENABLE = 1;
}

void rtc_disable() {
    RTC_ENABLE = 0;
}

// RTC Time Setters

void rtc_set_time(byte hours, byte minutes, byte seconds) {
    RTC_SECONDS = seconds;
    RTC_MINUTES = minutes;
    RTC_HOURS = hours;
}

void rtc_set_seconds(byte seconds) {
    RTC_SECONDS = seconds;
}

void rtc_set_minutes(byte minutes) {
    RTC_MINUTES = minutes;
}

void rtc_set_hours(byte hours) {
    RTC_HOURS = hours;
}

// RTC Time Getters

void rtc_get_time(byte* hours, byte* minutes, byte* seconds) {
    *seconds = RTC_SECONDS;
    *minutes = RTC_MINUTES;
    *hours = RTC_HOURS;
}

byte rtc_get_seconds() {
    return RTC_SECONDS;
}

byte rtc_get_minutes() {
    return RTC_MINUTES;
}

byte rtc_get_hours() {
    return RTC_HOURS;
}

// RTC Date Setters

void rtc_set_date(word year, byte month, byte day, byte week) {
    RTC_YEAR = (byte)(year - 2026); // Store as offset from 2026
    RTC_MONTH = month;
    RTC_DAY = day;
    RTC_WEEK = week;
}

void rtc_set_day(byte day) {
    RTC_DAY = day;
}

void rtc_set_month(byte month) {
    RTC_MONTH = month;
}

void rtc_set_year(word year) {
    RTC_YEAR = (byte)(year - 2026); // Store as offset from 2026
}

// RTC Date Getters

void rtc_get_date(word* year, byte* month, byte* day, byte* week) {
    *year = (word)(RTC_YEAR + 2026); // Retrieve as offset from 2026
    *month = RTC_MONTH;
    *day = RTC_DAY;
    *week = RTC_WEEK;
}

byte rtc_get_day() {
    return RTC_DAY;
}

byte rtc_get_month() {
    return RTC_MONTH;
}

word rtc_get_year() {
    return (word)(RTC_YEAR + 2026); // Retrieve as offset from 2026
}

// Delay Functions

// Internal delay function using Timer0
void __delay(word after_ticks) {
    if ((FCON & 2) != 0)
        FCON &= 0xfd;
    __DI();
    Timer0Interval = after_ticks;
    Timer0Counter = 0;
    Timer0Control = 0x0101;
    InterruptPending_W0 = 0;
    StopAcceptor = 0x50;
    StopAcceptor = 0xa0;
    StopControl = 2;
    __asm("nop");
    __asm("nop");
    __EI();
}

void delay_ms(word ms) {
    // Loop in 1000ms chunks to avoid tick overflow (max 0xFFFF)
    while (ms >= 1000) {
        __delay(1000 * TICKS_PER_MS);
        ms -= 1000;
    }
    if (ms > 0) {
        __delay(ms * TICKS_PER_MS);
    }
}

void delay_s(word s) {
    // Loop 1000ms at a time to avoid tick overflow (max 0xFFFF)
    for (word i = 0; i < s; i++) {
        delay_ms(1000);
    }
}

// Tick Conversion Functions

word ms_to_ticks(word ms) {
    return ms * TICKS_PER_MS;
}

word s_to_ticks(word s) {
    return s * TICKS_PER_SECOND;
}

word ticks_to_ms(word ticks) {
    return ticks / TICKS_PER_MS;
}

word ticks_to_s(word ticks) {
    // Due to ticks being a max of 0xFFFF, this only works for small values
    return ticks / TICKS_PER_SECOND;
}
