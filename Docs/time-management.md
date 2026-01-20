# Time Management

## Overview
This document describes Tangent's time management utilities, including RTC access, time formatting helpers, and millisecond/second delay and tick conversion helpers. The implementation targets the platform RTC and a software delay primitive built on Timer0.

## Concepts
- RTC registers: hardware registers are mapped via `RTC_*` macros in `src/time/time.h` (seconds, minutes, hours, day, week, month, year). These registers are encoded in BCD (binary-coded decimal) for display; `get_time_string` converts BCD to human-readable decimal in the selected format.
- Year offset: the RTC stores `RTC_YEAR` as an 8-bit offset relative to 2026. Use `rtc_set_year` / `rtc_get_year` which apply the offset.
- Drift: the onboard RTC does not use a crystal oscillator and may drift (~2 minutes/hour). Do not rely on it for precise timekeeping.
- Ticks: timer ticks are used for delay primitives. Constants: `TICKS_PER_MS = 8`, `TICKS_PER_SECOND = 8000`.

---

## API Reference

Headers: `src/time/time.h`

### Time string formatting
- `void get_time_string(format_t format, char* out)`
  - Produces a null-terminated time string in the provided buffer according to `format` (`TIME_FORMAT_24H`, `TIME_FORMAT_12H`, `TIME_FORMAT_24H_WITH_SECONDS`, `TIME_FORMAT_12H_WITH_SECONDS`).
  - Buffer requirement: allocate at least 16 bytes to be safe (e.g., "12:34:56 PM" + NUL).

### RTC control
- `void rtc_reset()` — Reset RTC registers to a safe default (0s/1s where applicable).
- `void rtc_enable()` — Enable RTC operation.
- `void rtc_disable()` — Disable RTC operation.

### RTC time setters/getters
- `void rtc_set_time(byte hours, byte minutes, byte seconds)`
- `void rtc_set_seconds(byte seconds)`
- `void rtc_set_minutes(byte minutes)`
- `void rtc_set_hours(byte hours)`
- `void rtc_get_time(byte* hours, byte* minutes, byte* seconds)`
- `byte rtc_get_seconds()` / `byte rtc_get_minutes()` / `byte rtc_get_hours()`

Notes: RTC registers are BCD-encoded. `get_time_string` decodes BCD for display. The public setters (`rtc_set_time`, `rtc_set_seconds`, etc.) write raw bytes directly to the RTC registers; callers must therefore supply BCD-encoded values (for example, decimal 13 → 0x13) or convert decimal values to BCD before calling.

### Decimal ↔ BCD helpers
If you prefer to use decimal values in application code, use small helpers to convert between decimal and BCD. Example implementations (copy into your codebase if needed):

```c
static byte dec_to_bcd(byte dec) {
    return (byte)(((dec / 10) << 4) | (dec % 10));
}

static byte bcd_to_dec(byte bcd) {
    return (byte)(((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F));
}

// Convenience wrappers that use decimal parameters and call the raw setters
static void rtc_set_time_dec(byte hours, byte minutes, byte seconds) {
    rtc_set_time(dec_to_bcd(hours), dec_to_bcd(minutes), dec_to_bcd(seconds));
}

static void rtc_get_time_dec(byte* hours, byte* minutes, byte* seconds) {
    byte h, m, s;
    rtc_get_time(&h, &m, &s);
    *hours = bcd_to_dec(h);
    *minutes = bcd_to_dec(m);
    *seconds = bcd_to_dec(s);
}
```

Consider adding these helpers to the public API (`time.h`) if you prefer decimal semantics across the codebase.

### RTC date setters/getters
- `void rtc_set_date(word year, byte month, byte day, byte week)`
- `void rtc_set_day(byte day)`
- `void rtc_set_month(byte month)`
- `void rtc_set_year(word year)`
- `void rtc_get_date(word* year, byte* month, byte* day, byte* week)`
- `byte rtc_get_day()` / `byte rtc_get_month()` / `word rtc_get_year()`

Notes: `rtc_set_year` accepts a full year (e.g., 2026); stored value is offset by 2026 internally. `rtc_get_year` returns the full year.

### Delay and tick conversions
- `void delay_ms(word ms)` — block for approximately `ms` milliseconds using Timer0 and platform ticks.
- `void delay_s(word s)` — block for `s` seconds (loops over `delay_ms`).
- `word ms_to_ticks(word ms)` — convert ms to timer ticks.
- `word s_to_ticks(word s)` — convert s to timer ticks.
- `word ticks_to_ms(word ticks)` — convert ticks to milliseconds.
- `word ticks_to_s(word ticks)` — convert ticks to seconds.

Notes: Delay resolution is limited by `TICKS_PER_MS` (8 ticks/ms) and underlying `__delay` implementation.

---

## Examples

### 1) Format and print current time
```c
char buf[16];
get_time_string(TIME_FORMAT_12H_WITH_SECONDS, buf);
// buf contains e.g. "12:34:56 PM"
// pass to display routine
```

### 2) Set RTC and read back
```c
// Set RTC to 13:05:07 (BCD-encoded values)
rtc_enable();
rtc_set_time(0x13, 0x05, 0x07);

byte h, m, s;
rtc_get_time(&h, &m, &s);
// h/m/s will contain the raw RTC register values (BCD). Use get_time_string to get readable form.
char out[16];
get_time_string(TIME_FORMAT_24H_WITH_SECONDS, out);
```

### 3) Delay example and conversions
```c
// Delay for 250ms
delay_ms(250);

// Convert 500ms to ticks
word ticks = ms_to_ticks(500);
// Convert back
word ms = ticks_to_ms(ticks);
```

---

## Caveats and implementation notes
- The RTC hardware may drift; for accurate time use an external reference or perform periodic synchronization.
- The RTC registers are stored in BCD format. `get_time_string` handles BCD decoding for display; setters write raw bytes to registers and thus expect compatible encoding.
- `rtc_set_year` stores a year offset relative to 2026.
- `__delay` (internal) uses Timer0 and disables interrupts briefly; avoid calling long blocking delays from critical interrupt contexts.
- TODO: Add alarm and callback APIs when platform alarm hardware is finalized.

---

## Cross-links and next steps
- Source: `src/time/time.h`, `src/time/time.c`.
- Suggested follow-ups: add a small helper `dec_to_bcd`/`bcd_to_dec` pair to the public API, add alarm APIs, add unit tests for conversion functions.

---

Document created as a first draft. Review for desired detail level and any additional examples to include.
