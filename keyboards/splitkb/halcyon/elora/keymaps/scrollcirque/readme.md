# scrollcirque

Vial keymap for the Halcyon Elora rev2 with a Cirque trackpad on each half.

- The left trackpad scrolls instead of moving the cursor, in both
  directions, with content following the finger. Side and speed are set in
  `config.h` (`CIRQUE_SCROLL_PAD_LEFT`, `CIRQUE_SCROLL_DIVISOR`); the
  direction is in `cirque_scroll.c`.
- Cursor glide (the cursor coasting after the finger lifts) and circular
  scroll are off, through `HLC_CIRQUE_NO_GLIDE` and
  `HLC_CIRQUE_NO_CIRCULAR_SCROLL`.
- Bootmagic: holding the top outer key of a half while plugging USB into
  that half starts the bootloader. See [Bootmagic](#bootmagic) below for
  which firmware has this.

## Before every flash: save the Vial layout

Every new Vial build resets the keymap stored on the keyboard to this
keymap's default. Vial checks the stored keymap against a build ID, and
vial-qmk generates that ID at random for each build (`util/build_id.py`,
used in `quantum/via.c`). So:

1. In Vial, **File → Save current layout** to a `.vil` file.
2. Flash.
3. **File → Load saved layout** to restore it, with USB in the half you
   normally use.

Each half stores its own keymap, and Vial reads and writes only the half
with the USB cable.

## Compiling

From the root of this repository:

```bash
git pull
qmk userspace-compile
```

This writes `elora_scrollcirque.uf2` to the repository root. The same file
goes on both halves: each half detects whether it is left or right from a
pin (`GP23`), not from the firmware.

## Flashing one half

Flash one half at a time, with the cable between the halves disconnected.
Never connect or disconnect that cable while USB is plugged in.

1. Put the half into bootloader mode; see the next section.
2. Check it is in bootloader mode:

   ```bash
   lsusb | grep -i 2e8a
   ```

   `2e8a:0003` (Raspberry Pi RP2 Boot) means it is. No line, or another ID,
   means the half is still running its firmware.
3. Mount the drive and copy the firmware:

   ```bash
   lsblk -o NAME,LABEL,SIZE,MOUNTPOINTS   # find the RPI-RP2 partition
   udisksctl mount -b /dev/sdX1           # prints the mount point
   cp elora_scrollcirque.uf2 /run/media/$USER/RPI-RP2/
   ```

   Mounting with `udisksctl` makes the drive writable without `sudo`. If
   it was mounted with `sudo mount`, the copy needs `sudo cp` and will ask
   for a password; make sure the copy actually finished.
4. Do not unplug until the drive disappears by itself: that is the half
   rebooting into the new firmware. `lsusb | grep -i 2e8a` should then no
   longer show `2e8a:0003`.

Flash both halves. Halves on different builds may not talk to each other,
because the Halcyon module adds its own messages between halves
(`SPLIT_TRANSACTION_IDS_KB` in `users/halcyon_modules/splitkb/config.h`).

## Getting into bootloader mode

Any of these, from most to least convenient:

- **Bootmagic**, once the half runs firmware that has it; see below.
- **A `QK_BOOT` key** in the keymap of the half that has the USB cable.
  The key only exists on a half whose own stored keymap contains it; see
  [Adding a QK_BOOT key](#adding-a-qk_boot-key-to-a-halfs-keymap).
- **Double-tapping the reset button** on the controller. The second tap must
  come within 200 ms of the first (`RP2040_BOOTLOADER_DOUBLE_TAP_RESET`,
  timeout from vial-qmk's `platforms/chibios/bootloaders/rp2040.c`).
- **The boot button**: hold it while plugging in USB. This works whatever
  firmware is installed, but on the left half the module has to come off to
  reach it.

## Adding a QK_BOOT key to a half's keymap

This works with the firmware already on the keyboard; nothing needs
rebuilding.

Vial only stores a `QK_BOOT` key while the keyboard is unlocked: on a locked
keyboard, the firmware replaces any `QK_BOOT` written to the keymap with an
invalid keycode (vial-qmk `quantum/nvm/eeprom/nvm_dynamic_keymap.c`). Unlocking
needs one key on each half held together: `VIAL_UNLOCK_COMBO_ROWS { 0, 5 }` and
`VIAL_UNLOCK_COMBO_COLS { 1, 1 }` in `config.h`, the innermost top-row key of
each half. So a half on its own can never be unlocked. Connect both halves
instead; the layout still lands only in the half with the USB cable.

1. Unplug USB, connect the cable between the halves, then plug USB into
   the half that needs the key.
2. In Vial (desktop app, or vial.rocks in a Chromium-based browser), pick
   a rarely used layer and assign **Reset** (`QK_BOOT`) from the
   **Quantum** tab to a key on that half. If it is not listed, use the
   **Any** key and type `QK_BOOT`. Vial asks to unlock: hold the two keys
   it highlights until the bar fills.
3. **File → Save current layout**, so the `.vil` file contains the key.
4. To give the other half the key too: unplug USB, plug it into the other
   half (cable between the halves still connected), unlock again if asked,
   and **File → Load saved layout** with that `.vil`.

Pressing a `QK_BOOT` key works whether or not the keyboard is unlocked; only
writing one needs the unlock. A key reboots the half it is on, so press the
one on the half with the USB cable.

## Bootmagic

Bootmagic is a QMK check that runs at power-up, before any keymap loads: if
one particular key is held, the half jumps to the bootloader. It needs no
`QK_BOOT` key, so it works on either half on its own after any flash.

QMK's default key position, matrix `[0, 0]`, has no key on the Elora, so
this keymap's `config.h` sets:

| Half  | Matrix position | Physical key  | Setting                                             |
| ----- | --------------- | ------------- | --------------------------------------------------- |
| Left  | `[0, 6]`        | top-left key  | `BOOTMAGIC_ROW 0`, `BOOTMAGIC_COLUMN 6`             |
| Right | `[5, 6]`        | top-right key | `BOOTMAGIC_ROW_RIGHT 5`, `BOOTMAGIC_COLUMN_RIGHT 6` |

These were added in commit `e5524ee`. Firmware built from that commit or
later has it; firmware built before it does not, and holding the key then
does nothing.

### Getting the Bootmagic firmware onto the keyboard

The firmware already on the keyboard has no working Bootmagic, so this one
flash has to use another way into the bootloader.

1. Save the Vial layout.
2. Compile, as above.
3. For each half, enter the bootloader with a `QK_BOOT` key (add one first
   if that half lacks it, as above) or the boot button, then flash it.
4. Reconnect the halves and load the saved layout.

### Using it

1. Unplug USB and disconnect the cable between the halves.
2. Hold the half's top outer key.
3. Plug USB into that half, then release the key.
4. Check with `lsusb | grep -i 2e8a` and flash as above.

Bootmagic also clears that half's stored Vial keymap, so load the saved
layout afterwards. If holding the key does not start the bootloader on
firmware built from `e5524ee` or later, the matrix positions above are
wrong; the boot button still works.

## Splitkb documentation

- [Halcyon firmware](https://docs.splitkb.com/product-guides/halcyon-series/using/firmware)
- [Flashing troubleshooting](https://docs.splitkb.com/troubleshooting/flashing)
- [Resetting your controller's EEPROM](https://docs.splitkb.com/resources/reset-eeprom)
