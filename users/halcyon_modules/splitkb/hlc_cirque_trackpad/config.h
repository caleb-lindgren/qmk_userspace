// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

// Any QMK options should go here

#pragma once

#define HLC_CIRQUE_TRACKPAD

#define CIRQUE_PINNACLE_DIAMETER_MM 35
#undef POINTING_DEVICE_CS_PIN
#define POINTING_DEVICE_CS_PIN 13
#define POINTING_DEVICE_ROTATION_180
#define CIRQUE_PINNACLE_CURVED_OVERLAY

// A keymap's config.h is read before this file, so a keymap can define
// HLC_CIRQUE_NO_GLIDE or HLC_CIRQUE_NO_CIRCULAR_SCROLL to opt out of these.
#ifndef HLC_CIRQUE_NO_GLIDE
#    define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
#endif
#define CIRQUE_PINNACLE_POSITION_MODE CIRQUE_PINNACLE_ABSOLUTE_MODE
#define CIRQUE_PINNACLE_TAP_ENABLE
#ifndef HLC_CIRQUE_NO_CIRCULAR_SCROLL
#    define POINTING_DEVICE_GESTURES_SCROLL_ENABLE
#endif
