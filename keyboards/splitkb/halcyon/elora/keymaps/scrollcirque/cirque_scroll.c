// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#ifndef CIRQUE_SCROLL_PAD_LEFT
#    define CIRQUE_SCROLL_PAD_LEFT true
#endif
#ifndef CIRQUE_SCROLL_DIVISOR
#    define CIRQUE_SCROLL_DIVISOR 8
#endif

// Movement left over after dividing, carried into the next report so slow
// finger movement still scrolls instead of being rounded away.
static int16_t scroll_remainder_h = 0;
static int16_t scroll_remainder_v = 0;

// Runs on the master half with both halves' reports; turn one trackpad's
// movement into scrolling before the reports are merged.
report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    report_mouse_t *scroll = CIRQUE_SCROLL_PAD_LEFT ? &left_report : &right_report;

    // Mouse y grows downwards and wheel v grows upwards, so negate y: moving
    // the finger down scrolls down, like a mouse wheel. Negate x as well, or
    // drop the minus on y, to reverse a direction.
    scroll_remainder_h += scroll->x;
    scroll_remainder_v -= scroll->y;

    scroll->h = scroll_remainder_h / CIRQUE_SCROLL_DIVISOR;
    scroll->v = scroll_remainder_v / CIRQUE_SCROLL_DIVISOR;
    scroll_remainder_h -= scroll->h * CIRQUE_SCROLL_DIVISOR;
    scroll_remainder_v -= scroll->v * CIRQUE_SCROLL_DIVISOR;
    scroll->x = 0;
    scroll->y = 0;

    return pointing_device_combine_reports(left_report, right_report);
}
