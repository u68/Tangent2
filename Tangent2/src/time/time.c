/*
 * time.c
 * Implementation of time management functions
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#include "time.h"

// Convert BCD byte to decimal value
static byte bcd_to_dec(byte bcd) {
    return ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);
}

// Convert decimal byte to ASCII string
static void dec_to_ascii(byte dec, char* out) {
    out[0] = '0' + ((dec / 10) % 10);
    out[1] = '0' + (dec % 10);
    out[2] = 0;
}

// Check if hours is in PM (afternoon)
static byte is_pm(byte hours) {
    return hours >= 12;
}

// Convert 24h format to 12h format
static byte hours_12h(byte hours_24h) {
    if (hours_24h == 0) return 12;
    if (hours_24h > 12) return hours_24h - 12;
    return hours_24h;
}

// Format time in 24h format with optional seconds
static void format_24h_time(byte hours, byte minutes, byte seconds, byte show_seconds, char* out) {
    byte pos = 0;
    
    dec_to_ascii(bcd_to_dec(hours), out + pos);
    pos += 2;
    out[pos++] = ':';
    dec_to_ascii(bcd_to_dec(minutes), out + pos);
    
    if (show_seconds) {
        pos += 2;
        out[pos++] = ':';
        dec_to_ascii(bcd_to_dec(seconds), out + pos);
        pos += 2;
    }
    
    out[pos] = 0;
}

// Format time in 12h format with AM/PM and optional seconds
static void format_12h_time(byte hours, byte minutes, byte seconds, byte show_seconds, char* out) {
    byte pos = 0;
    
    dec_to_ascii(hours_12h(bcd_to_dec(hours)), out + pos);
    pos += 2;
    out[pos++] = ':';
    dec_to_ascii(bcd_to_dec(minutes), out + pos);
    pos += 2;
    
    if (show_seconds) {
        out[pos++] = ':';
        dec_to_ascii(bcd_to_dec(seconds), out + pos);
        pos += 2;
    }
    
    out[pos++] = ' ';
    
    if (is_pm(bcd_to_dec(hours))) {
        out[pos++] = 'P';
        out[pos++] = 'M';
    } else {
        out[pos++] = 'A';
        out[pos++] = 'M';
    }
    
    out[pos] = 0;
}

// Get time string in specified format from RTC
void get_time_string(format_t format, char* out) {
    byte hours_bcd = RTC_HOURS;
    byte minutes_bcd = RTC_MINUTES;
    byte seconds_bcd = RTC_SECONDS;
    
    switch (format) {
    case TIME_FORMAT_24H:
        format_24h_time(hours_bcd, minutes_bcd, seconds_bcd, 0, out);
        break;
    case TIME_FORMAT_12H:
        format_12h_time(hours_bcd, minutes_bcd, seconds_bcd, 0, out);
        break;
    case TIME_FORMAT_24H_WITH_SECONDS:
        format_24h_time(hours_bcd, minutes_bcd, seconds_bcd, 1, out);
        break;
    case TIME_FORMAT_12H_WITH_SECONDS:
        format_12h_time(hours_bcd, minutes_bcd, seconds_bcd, 1, out);
        break;
    }
}

// Reset RTC to default state
void rtc_reset(void) {
    RTC_ENABLE = 0;
    RTC_SECONDS = 0;
    RTC_MINUTES = 0;
    RTC_HOURS = 0;
    RTC_DAY = 1;
    RTC_WEEK = 1;
    RTC_MONTH = 1;
    RTC_YEAR = 0;
}

// Enable RTC
void rtc_enable(void) {
    RTC_ENABLE = 1;
}

// Disable RTC
void rtc_disable(void) {
    RTC_ENABLE = 0;
}

// Set full time (hours, minutes, seconds)
void rtc_set_time(byte hours, byte minutes, byte seconds) {
    RTC_SECONDS = seconds;
    RTC_MINUTES = minutes;
    RTC_HOURS = hours;
}

// Set seconds register
void rtc_set_seconds(byte seconds) {
    RTC_SECONDS = seconds;
}

// Set minutes register
void rtc_set_minutes(byte minutes) {
    RTC_MINUTES = minutes;
}

// Set hours register
void rtc_set_hours(byte hours) {
    RTC_HOURS = hours;
}

// Get full time (hours, minutes, seconds)
void rtc_get_time(byte* hours, byte* minutes, byte* seconds) {
    *seconds = RTC_SECONDS;
    *minutes = RTC_MINUTES;
    *hours = RTC_HOURS;
}

// Get seconds register
byte rtc_get_seconds() {
    return RTC_SECONDS;
}

// Get minutes register
byte rtc_get_minutes() {
    return RTC_MINUTES;
}

// Get hours register
byte rtc_get_hours() {
    return RTC_HOURS;
}

// Set full date (year, month, day, week)
void rtc_set_date(word year, byte month, byte day, byte week) {
    RTC_YEAR = (byte)(year - 2026);
    RTC_MONTH = month;
    RTC_DAY = day;
    RTC_WEEK = week;
}

// Set day register
void rtc_set_day(byte day) {
    RTC_DAY = day;
}

// Set month register
void rtc_set_month(byte month) {
    RTC_MONTH = month;
}

// Set year register
void rtc_set_year(word year) {
    RTC_YEAR = (byte)(year - 2026);
}

// Get full date (year, month, day, week)
void rtc_get_date(word* year, byte* month, byte* day, byte* week) {
    *year = (word)(RTC_YEAR + 2026);
    *month = RTC_MONTH;
    *day = RTC_DAY;
    *week = RTC_WEEK;
}

// Get day register
byte rtc_get_day() {
    return RTC_DAY;
}

// Get month register
byte rtc_get_month() {
    return RTC_MONTH;
}

// Get year register (with 2026 offset)
word rtc_get_year() {
    return (word)(RTC_YEAR + 2026);
}

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

// Delay for specified milliseconds
void delay_ms(word ms) {
    while (ms >= 1000) {
        __delay(1000 * TICKS_PER_MS);
        ms -= 1000;
    }
    if (ms > 0) {
        __delay(ms * TICKS_PER_MS);
    }
}

// Delay for specified seconds
void delay_s(word s) {
    for (word i = 0; i < s; i++) {
        delay_ms(1000);
    }
}

// Convert milliseconds to timer ticks
word ms_to_ticks(word ms) {
    return ms * TICKS_PER_MS;
}

// Convert seconds to timer ticks
word s_to_ticks(word s) {
    return s * TICKS_PER_SECOND;
}

// Convert timer ticks to milliseconds
word ticks_to_ms(word ticks) {
    return ticks / TICKS_PER_MS;
}

// Convert timer ticks to seconds
word ticks_to_s(word ticks) {
    return ticks / TICKS_PER_SECOND;
}
