// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "sylvain"
#endif

#include "Kaleidoscope.h"
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LEDEffect-BootGreeting.h"
#include "Kaleidoscope-LEDEffect-Breathe.h"
#include "Kaleidoscope-LEDEffect-Rainbow.h"
#include "Kaleidoscope-LED-Palette-Theme.h"
#include "Kaleidoscope-LED-ActiveModColor.h"
#include "Kaleidoscope-Colormap.h"
#include "Kaleidoscope-HardwareTestMode.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-MagicCombo.h"
#include "Kaleidoscope-USB-Quirks.h"
#include "Kaleidoscope-Qukeys.h"

// macros
enum {
  MACRO_TOGGLE_QUKEYS
};


// layers
enum {
  PRIMARY,
  FUNCTION
};

// *INDENT-OFF*

KEYMAPS(

// primary layer
  [PRIMARY] = KEYMAP_STACKED(
    ___,           Key_1,  Key_2,  Key_3,  Key_4,  Key_5,  Key_LEDEffectNext,
    Key_Backtick,  Key_Q,  Key_W,  Key_E,  Key_R,  Key_T,  Key_Tab,
    Key_PageUp,    Key_A,  Key_S,  Key_D,  Key_F,  Key_G,  /*---*/
    Key_PageDown,  Key_Z,  Key_X,  Key_C,  Key_V,  Key_B,  Key_Escape,
    //
    Key_LeftControl, Key_Backspace, Key_LeftGui, Key_LeftShift,
    //
    ShiftToLayer(FUNCTION),
    //============================================================================================
    Key_LEDEffectNext,       Key_6,  Key_7,  Key_8,      Key_9,       Key_0,          ___,
    Key_Enter,               Key_Y,  Key_U,  Key_I,      Key_O,       Key_P,          Key_Equals,
    /*---*/                  Key_H,  Key_J,  Key_K,      Key_L,       Key_Semicolon,  Key_Quote,
    Key_RightAlt,            Key_N,  Key_M,  Key_Comma,  Key_Period,  Key_Slash,      Key_Minus,
    //
    Key_RightShift,  Key_LeftAlt,  Key_Spacebar,  Key_RightControl,
    //
    ShiftToLayer(FUNCTION)),

// fn layer
  [FUNCTION] =  KEYMAP_STACKED(
    ___,       Key_F1,           Key_F2,      Key_F3,        Key_F4,      Key_F5,   Key_CapsLock,
    Key_Tab,   ___,              Key_Home,    Key_PageDown,  Key_PageUp,  Key_End,  ___,
    Key_Home,  ___,              ___,         ___,           ___,         ___,      /*---*/
    Key_End,   Key_PrintScreen,  Key_Insert,  ___,           ___,         ___,      ___,
    //
    ___,  Key_Delete,  ___,  ___,
    //
   ___,
    //============================================================================================
    Consumer_ScanPreviousTrack,  Key_F6,                  Key_F7,                    Key_F8,                    Key_F9,           Key_F10,           Key_F11,
    Consumer_PlaySlashPause,     Consumer_ScanNextTrack,  Key_LeftCurlyBracket,      Key_RightCurlyBracket,     Key_LeftBracket,  Key_RightBracket,  Key_F12,
    /*---*/                      Key_LeftArrow,           Key_DownArrow,             Key_UpArrow,               Key_RightArrow,   ___,               ___,
    Key_PcApplication,           Consumer_Mute,           Consumer_VolumeDecrement,  Consumer_VolumeIncrement,  ___,              Key_Backslash,     Key_Pipe,
    //
    ___,  Key_LeftGui,  Key_Enter,  ___,
    //
    ___)
) // KEYMAPS
// *INDENT-ON*

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  return MACRO_NONE;
}

// host power management
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
    LEDControl.disable();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.enable();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    break;
  }
}

void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

// magic combos
enum {
  // Toggle between Boot (6-key rollover; for BIOSes and early boot) and NKRO
  // mode.
  COMBO_TOGGLE_NKRO_MODE,
  // Enter test mode
  COMBO_ENTER_TEST_MODE
};

/** Wrappers, to be used by MagicCombo. **/

/**
 * This simply toggles the keyboard protocol via USBQuirks, and wraps it within
 * a function with an unused argument, to match what MagicCombo expects.
 */
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}

/**
 *  This enters the hardware test mode
 */
static void enterHardwareTestMode(uint8_t combo_index) {
  HardwareTestMode.runTests();
}

// magic combos key mappings
USE_MAGIC_COMBOS({
  .action = toggleKeyboardProtocol, .keys = { R3C6, R2C6, R3C7 } // Left Fn + Esc + Shift
}, {
  .action = enterHardwareTestMode, .keys = { R3C6, R0C0, R0C6 } // Left Fn + Prog + LED
});

KALEIDOSCOPE_INIT_PLUGINS(
  Qukeys,
  EEPROMSettings,
  EEPROMKeymap,
  Focus,
  FocusSettingsCommand,
  FocusEEPROMCommand,
  BootGreetingEffect,
  HardwareTestMode,

  // led effects
  LEDControl,
  LEDOff,
  LEDRainbowEffect,
  LEDRainbowWaveEffect,
  LEDBreatheEffect,
  LEDPaletteTheme,
  ColormapEffect,
  ActiveModColorEffect,

  // The macros plugin adds support for macros
  Macros,

  HostPowerManagement,
  MagicCombo,
  USBQuirks
);

void setup() {
  QUKEYS(
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 2), Key_LeftAlt),        // S/alt
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 3), Key_LeftControl),    // D/ctrl
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 4), Key_LeftShift),      // F/shift
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 11), Key_RightShift),    // J/shift
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 12), Key_RightControl),  // K/ctrl
    kaleidoscope::plugin::Qukey(0, KeyAddr(2, 13), Key_LeftAlt),       // L/alt
  )

  // small overlap to allow some flexibility
  Qukeys.setOverlapThreshold(95);
  // short delay for alternate keys, in an attempt to reduce the lag
  Qukeys.setHoldTimeout(200);

  // experiment other timing parameters (not supported yet)
  // Qukeys.setMinimumHoldTime(100); // default 50 delay to activate alt key, will require slow down for modifiers

  // First, call Kaleidoscope's internal setup function
  Kaleidoscope.setup();

  // We set the brightness of the rainbow effects to 150 (on a scale of 0-255)
  // This draws more than 500mA, but looks much nicer than a dimmer effect
  LEDRainbowEffect.brightness(150);
  LEDRainbowWaveEffect.brightness(150);

  // Set the action key the test mode should listen for to Left Fn
  HardwareTestMode.setActionKey(R3C6);

  // start with LEDs off
  LEDOff.activate();

  EEPROMKeymap.setup(2);

  ColormapEffect.max_layers(5);

  ActiveModColorEffect.highlight_color = CRGB(0x00, 0xff, 0xff);

  Qukeys.activate();
}

void loop() {
  Kaleidoscope.loop();
}
