# scrollcirque

Vial keymap for the Halcyon Elora rev2 with a Cirque trackpad on each half.

- One trackpad scrolls instead of moving the cursor, in both directions,
  with content following the finger. It is currently the right one, swapped
  from the left to test whether the right trackpad's erratic cursor
  movement follows the hardware. Side and speed are set in `config.h`
  (`CIRQUE_SCROLL_PAD_LEFT`, `CIRQUE_SCROLL_DIVISOR`); the direction is in
  `cirque_scroll.c`.
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

## Investigation: erratic cursor trackpad (2026-11-18 to 11-21)

### Symptoms

Only the cursor trackpad (the right one, before the swap) is affected,
at random times, for minutes at a stretch. The cursor stops following the
finger: it shakes in place or slides at right angles to the finger's
movement, then sometimes jumps far in the finger's direction. It also
clicks without a tap, during episodes and sometimes during otherwise
normal movement. Afterwards, typing for a while or stepping away usually
brings it back to normal.

### Setup

- USB goes into the right half, so the right half is the master and reads
  its own trackpad. The right trackpad's data never crosses the cable
  between the halves, which rules that cable out.
- The keyboard is on a USB hub that also carries HDMI to the monitor. The
  hub plugs into the laptop's USB-C port. The monitor's power cord has an
  earth pin. The laptop charger is ungrounded and runs the laptop directly
  when plugged in.
- The same hub, monitor and charger showed none of this with a holykeebs
  keyboard using Azoteq TPS43 trackpads.

### Observations

- Scrolling on the left trackpad stayed normal during every episode.
- For a while on 2026-11-19, unplugging and replugging USB fixed an
  episode. Later that day, none of the fixes worked consistently: not
  replugging, not unplugging the charger, not touching something earthed.

### Theories

1. **Flat cable between the trackpad and the module, damaged or not fully
   seated.** This turned out to be the cause. The cable carries the trackpad
   chip's power, ground and data lines.
   - A data line that cuts in and out garbles readings into jumps,
     dropouts and phantom taps. The tap code (vial-qmk
     `drivers/sensors/cirque_pinnacle_gestures.c`) counts any lift that
     comes soon after a touch as a click.
   - A dropout on power or ground restarts the chip with its factory
     settings. The driver (`drivers/sensors/cirque_pinnacle.c`) configures
     the chip once in `cirque_pinnacle_init` and never checks it again, so
     the keyboard keeps decoding readings in the wrong format until the
     next power-up.
   - That fits replugging USB working at first and then not at all, as the
     contact got worse.
2. **The trackpad chip's baseline drifting.** The chip keeps a reading of
   what "no finger" looks like; if that goes wrong, positions bend and
   touches flicker, until the chip re-measures while nothing touches the
   pad. This explains episodes ending by themselves, but not the problem
   getting worse over the day.
3. **Noise from the ungrounded charger.** Unlikely: the earthed monitor
   grounds the laptop through the HDMI shield and the hub. That only holds
   if the monitor's earthed cord goes into the monitor, not into a
   separate power brick. Unplugging the charger during an episode did not
   reliably fix it.

### Resolution (2026-11-21)

Theory 1 was right. The right trackpad's flat cable had once been jarred;
replacing the cable fixed the trackpad.

Commit `4b0c182` had swapped the roles as a test, so the right trackpad
still scrolls and the left moves the cursor. Swapping back is a keymap
change, not a fix.

The firmware ideas from the investigation were not needed for this fault
and were not started. They remain options:

- halve the cursor speed (`CIRQUE_PINNACLE_DEFAULT_SCALE` is 1024 across
  the pad, about 29 units per mm on the 35 mm pad);
- a dead zone at the start of each touch, so taps and double-taps do not
  move the cursor;
- periodic checks that set the chip up again if its settings have reset,
  which would let the keyboard recover from a brief power dropout on the
  cable without a replug;
- logging the trackpad's readings to `qmk console`.

## Splitkb documentation

- [Halcyon firmware](https://docs.splitkb.com/product-guides/halcyon-series/using/firmware)
- [Flashing troubleshooting](https://docs.splitkb.com/troubleshooting/flashing)
- [Resetting your controller's EEPROM](https://docs.splitkb.com/resources/reset-eeprom)
