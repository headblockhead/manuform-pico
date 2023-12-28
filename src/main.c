// Minimal possible keyboard firmware for a 5 key macropad. (GPIOs 0-4)
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "usb_descriptors.h"

#include <squirrel.h>

// Send a HID report with the given keycodes to the host.
static void send_hid_kbd_codes(uint8_t keycode_assembly[6]) {
  // skip if hid is not ready yet
  if (!tud_hid_ready()) {
    return;
  };
  tud_hid_keyboard_report(
      REPORT_ID_KEYBOARD, modifiers,
      keycode_assembly); // Send the report. A report can be for a keyboard,
                         // mouse, joystick etc. In a keyboard report, the first
                         // argument is the report ID, the second is the
                         // modifiers (ctrl, shift, alt etc.). These are stored
                         // as a bitfield. The third is the keycodes to send.
                         // All keycodes that are sent are considered currently
                         // pressed. Detecting key presses and releases is done
                         // by the host. The only requirement from the firmware
                         // is that it sends a report with all currently pressed
                         // keys every 10ms.
}

// Send a HID report with no keycodes to the host.
static void send_hid_kbd_null() {
  // skip if hid is not ready yet
  if (!tud_hid_ready()) {
    return;
  };
  tud_hid_keyboard_report(
      REPORT_ID_KEYBOARD, modifiers,
      NULL); // Send a report with no keycodes. (no keys pressed)
}

// Every 10ms, we will sent 1 report for each HID device. In this case, we only
// have 1 HID device, the keyboard.
void hid_task(void) {
  // Poll every 10ms
  const uint32_t interval_ms = 10;    // Time between each report
  static uint32_t next_report_ms = 0; // Time of next report

  if (board_millis() - next_report_ms < interval_ms) { // If we are running too
                                                       // fast, return.
    return;
  };
  next_report_ms += interval_ms; // Schedule next report

  // Reports are sent in a chain, with one report for each HID device.

  // First, send the keyboard report. In a keyboard report, 6 keycodes can be
  // registered as pressed at once. A keycode is a number that represents a key
  // (such as 'a', 'b' or capslock).

  uint8_t keycode_assembly[6] = {
      0}; // The keycodes to send in the report. A max
          // of 6 keycodes can be regisered as currently pressed at once.
  uint_fast8_t index = 0; // The index of the keycode_assembly array.

  for (int i = 0; i <= 0xFF; i++) { // Loop through all keycodes.
    if (active_keycodes[i]) { // If the keycode is registered as active (pressed
                              // down),
      keycode_assembly[index] = i; // Add the keycode to the assembly array.
      index++;                     // Increment the index of the assembly array.
      if (index >= 6) { // If the report is full, stop adding keycodes. (this
                        // ignores any keycodes after the 6th active keycode)
        break;
      }
    }
  }
  if (index > 0) { // If there are any keycodes to send, send them.
    send_hid_kbd_codes(keycode_assembly);
  } else {
    send_hid_kbd_null();
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  // This callback is not used, but is required by tinyusb.
  (void)instance;
  (void)report;
  (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  // This callback is not used, but is required by tinyusb.
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  // This callback is not used, but is required by tinyusb.
  // Here you can receive data sent from the host to the device (such as
  // capslock LEDs, etc.)
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}

struct key key1 = {
    .rising = {mod_down},
    .risingargs = {KEYBOARD_MODIFIER_LEFTCTRL},
    .falling = {mod_up},
    .fallingargs = {KEYBOARD_MODIFIER_LEFTCTRL},
};

struct key key2 = {
    .rising = {mod_down},
    .risingargs = {KEYBOARD_MODIFIER_LEFTALT},
    .falling = {mod_up},
    .fallingargs = {KEYBOARD_MODIFIER_LEFTALT},
};

struct key key3 = {
    .rising = {key_down},
    .risingargs = {HID_KEY_C},
    .falling = {key_up},
    .fallingargs = {HID_KEY_C},
};

struct key key4 = {
    .rising = {key_down},
    .risingargs = {HID_KEY_D},
    .falling = {key_up},
    .fallingargs = {HID_KEY_D},
};

struct key key5 = {
    .rising = {key_down},
    .risingargs = {HID_KEY_E},
    .falling = {key_up},
    .fallingargs = {HID_KEY_E},
};

struct key *keys[5] = {&key1, &key2, &key3, &key4, &key5};

// The main function, runs tinyusb and the key scanning loop.
int main(void) {
  board_init(); // Initialize the pico board
  tusb_init();  // Initialize tinyusb

  for (int i = 0; i <= 4; i++) { // Initialize all key gpios
    gpio_init(i);
    gpio_set_dir(i, GPIO_IN);
    gpio_pull_down(i);
  }

  while (true) {
    tud_task();                    // tinyusb device task.
    hid_task();                    // Send HID reports to the host.
    for (int i = 0; i <= 4; i++) { // Loop through all keys
      bool gpio = gpio_get(i);     // Get the state of the key's gpio.
      check_key(keys[i],
                gpio); // Check if the key has changed state, and execute the
                       // key's rising or falling function if it has.
    }
  }
}
