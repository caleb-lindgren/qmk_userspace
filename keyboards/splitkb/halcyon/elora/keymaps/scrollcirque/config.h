// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define VIAL_KEYBOARD_UID {0x91, 0xC0, 0x72, 0xB1, 0xEA, 0x3F, 0xF0, 0xBA}

#define VIAL_UNLOCK_COMBO_ROWS { 0, 5 }
#define VIAL_UNLOCK_COMBO_COLS { 1, 1 }

// Increase the EEPROM size for layout options
#define VIA_EEPROM_LAYOUT_OPTIONS_SIZE 2

#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES

#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// Trackpads: no inertial glide after lifting the finger, no circular scroll
#define HLC_CIRQUE_NO_GLIDE
#define HLC_CIRQUE_NO_CIRCULAR_SCROLL

// Which half's trackpad scrolls instead of moving the cursor (see
// cirque_scroll.c): true for the left half, false for the right.
#define CIRQUE_SCROLL_PAD_LEFT true
// Trackpad movement per scroll step; raise it to scroll slower.
#define CIRQUE_SCROLL_DIVISOR 8
