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
#include "Kaleidoscope-IdleLEDs.h"

// macros
enum {
  MACRO_TOGGLE_QUKEYS,
  MACRO_RESET
};


// layers
enum {
  PRIMARY,
  FUNCTION,
  MAGIC
};

// *INDENT-OFF*

KEYMAPS(

// primary layer
  [PRIMARY] = KEYMAP_STACKED(
    ___,           Key_1,  Key_2,           Key_3,     Key_4,     Key_5,  Key_Hyper,
    Key_Backtick,  Key_Q,  MT(RightAlt,W),  Key_E,     Key_R,     Key_T,  Key_Tab,
    Key_PageUp,    Key_A,  ALT_T(S),        CTL_T(D),  SFT_T(F),  Key_G,  /*---*/
    Key_PageDown,  Key_Z,  Key_X,           Key_C,     Key_V,     Key_B,  Key_Escape,
    //
    Key_LeftControl, Key_Backspace, Key_LeftGui, Key_LeftShift,
    //
    LT(FUNCTION, Spacebar),
    //============================================================================================
    Key_Hyper,  Key_6,  Key_7,     Key_8,      Key_9,           Key_0,          M(MACRO_RESET),
    Key_Enter,  Key_Y,  Key_U,     Key_I,      MT(RightAlt,O),  Key_P,          Key_Equals,
    /*---*/     Key_H,  SFT_T(J),  CTL_T(K),   ALT_T(L),        Key_Semicolon,  Key_Quote,
    Key_Tab,    Key_N,  Key_M,     Key_Comma,  Key_Period,      Key_Slash,      Key_Minus,
    //
    Key_RightShift,  Key_LeftAlt,  Key_Spacebar,  Key_RightControl,
    //
    LT(FUNCTION, Spacebar)),

// fn layer
  [FUNCTION] =  KEYMAP_STACKED(
    ___,       Key_F1,           Key_F2,      Key_F3,                      Key_F4,                    Key_F5,                   Key_LEDEffectNext,
    Key_Tab,   ___,              ___,         Consumer_ScanPreviousTrack,  Consumer_ScanNextTrack,    Consumer_PlaySlashPause,  ___,
    Key_Home,  ___,              ___,         ___,                         ___,                       ___,                      /*---*/
    Key_End,   Key_PrintScreen,  Key_Insert,  Consumer_VolumeDecrement,    Consumer_VolumeIncrement,  Consumer_Mute,            ___,
    //
    ___,  Key_Delete,  ___,  ___,
    //
    ___,
    //============================================================================================
    ___,                  Key_F6,         Key_F7,                Key_F8,                 Key_F9,           Key_F10,           Key_F11,
    ___,                  ___,            Key_LeftCurlyBracket,  Key_RightCurlyBracket,  Key_LeftBracket,  Key_RightBracket,  Key_F12,
    /*---*/               Key_LeftArrow,  Key_DownArrow,         Key_UpArrow,            Key_RightArrow,   ___,               ___,
    Key_PcApplication,    Key_Home,       Key_PageDown,          Key_PageUp,             Key_End,          Key_Backslash,     Key_Pipe,
    //
    ShiftToLayer(MAGIC),  Key_LeftGui,    Key_Enter,             ___,
    //
    ___),

// magic layer
  [MAGIC] =  KEYMAP_STACKED(
    XXX,  Key_F13,  Key_F14,  Key_F15,  Key_F16,  Key_F17,  XXX,
    XXX,  XXX,      XXX,      XXX,      XXX,      XXX,      XXX,
    XXX,  XXX,      XXX,      XXX,      XXX,      XXX,      /**/
    XXX,  XXX,      XXX,      XXX,      XXX,      XXX,      XXX,
    //
    XXX,  XXX,      XXX,      XXX,
    //
    XXX,
    //============================================================================================
    XXX,  Key_F18,  Key_F19,  Key_F20,  Key_F21,  Key_F22,  Key_F23,
    XXX,  XXX,      XXX,      XXX,      XXX,      XXX,      Key_F24,
    /**/  XXX,      XXX,      XXX,      XXX,      XXX,      XXX,
    XXX,  XXX,      XXX,      XXX,      XXX,      XXX,      XXX,
    //
    XXX,  XXX,      XXX,      XXX,
    //
    XXX)


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

/**
   This simply toggles the keyboard protocol via USBQuirks, and wraps it within
   a function with an unused argument, to match what MagicCombo expects.
*/
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}

/**
    This enters the hardware test mode
*/
static void enterHardwareTestMode(uint8_t combo_index) {
  // Hardware test mode
  // use Left Fn+Prog+Led to enter
  // Hit left Fn many times to cycle through solid colors
  // Then you get individual key test mode, then hit every key multiple times
  // blue = pressed-down, red = key chatter detected, green = ok
  HardwareTestMode.runTests();
}

// magic combos key mappings
USE_MAGIC_COMBOS({
  .action = toggleKeyboardProtocol, .keys = { R3C6, R2C6, R3C7 } // Left Fn + Esc + Shift
}, {
  .action = enterHardwareTestMode, .keys = { R3C6, R0C0, R0C6 } // Left Fn + Prog + LED
});

KALEIDOSCOPE_INIT_PLUGINS(
  BootGreetingEffect,
  Qukeys,
  EEPROMSettings,
  EEPROMKeymap,
  Focus,
  FocusSettingsCommand,
  FocusEEPROMCommand,
  HardwareTestMode,

  // led effects
  LEDControl,
  IdleLEDs,
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

  // longer boot effect for easier visibility
  BootGreetingEffect.timeout = 10000;

  // require "long" hold to get alternate qukey
  // helps prevent unintentional alternate values (in this case it's modifier keys).
  Qukeys.setMinimumHoldTime(80);

  // try to keep it quite strict
  Qukeys.setOverlapThreshold(100);

  // disable tap repeat
  // Qukeys.setMaxIntervalForTapRepeat(0); // only available in 1.99.4+

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

  EEPROMKeymap.setup(3);

  ColormapEffect.max_layers(3);

  ActiveModColorEffect.setHighlightColor(CRGB(0x84, 0x00, 0xff)); // only available for 1.99.4+
  // ActiveModColorEffect.highlight_color = CRGB(0x84, 0x00, 0xff);

  Qukeys.activate();
}

void loop() {
  Kaleidoscope.loop();
}
