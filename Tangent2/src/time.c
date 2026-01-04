/*
 * time.c
 * Implementation of time management functions
 *  Created on: Jan 5, 2026
 *      Author: harma
 */

#include "time.h"

// RTC Control Functions

void rtc_reset() {
    // TODO: implement
}

void rtc_enable() {
    // TODO: implement
}

void rtc_disable() {
    // TODO: implement
}

// RTC Time Setters

void rtc_set_time(byte hour, byte minute, byte second) {
    // TODO: implement
}

void rtc_set_seconds(byte second) {
    // TODO: implement
}

void rtc_set_minutes(byte minute) {
    // TODO: implement
}

void rtc_set_hours(byte hour) {
    // TODO: implement
}

// RTC Time Getters

void rtc_get_time(byte* hour, byte* minute, byte* second) {
    // TODO: implement
}

byte rtc_get_seconds() {
    // TODO: implement
    return 0;
}

byte rtc_get_minutes() {
    // TODO: implement
    return 0;
}

byte rtc_get_hours() {
    // TODO: implement
    return 0;
}

// RTC Date Setters

void rtc_set_date(word year, byte month, byte day, byte week) {
    // TODO: implement
}

void rtc_set_day(byte day) {
    // TODO: implement
}

void rtc_set_month(byte month) {
    // TODO: implement
}

void rtc_set_year(word year) {
    // TODO: implement
}

// RTC Date Getters

void rtc_get_date(word* year, byte* month, byte* day, byte* week) {
    // TODO: implement
}

byte rtc_get_day() {
    // TODO: implement
    return 0;
}

byte rtc_get_month() {
    // TODO: implement
    return 0;
}

word rtc_get_year() {
    // TODO: implement
    return 0;
}

// Delay Functions

void delay_ms(word ms) {
    // TODO: implement
}

void delay_s(word s) {
    // TODO: implement
}

// Tick Conversion Functions

void ms_to_ticks(word ms, word* ticks) {
    // TODO: implement
}

void s_to_ticks(word s, word* ticks) {
    // TODO: implement
}

word ticks_to_ms(word ticks) {
    // TODO: implement
    return 0;
}

word ticks_to_s(word ticks) {
    // TODO: implement
    return 0;
}
