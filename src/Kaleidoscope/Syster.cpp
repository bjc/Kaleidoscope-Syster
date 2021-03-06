/* -*- mode: c++ -*-
 * Kaleidoscope-Syster -- Symbolic input system
 * Copyright (C) 2017, 2018  Gergely Nagy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Kaleidoscope-Syster.h>
#include <kaleidoscope/hid.h>

#undef SYSTER

namespace kaleidoscope {

// --- state ---
char Syster::symbol_[SYSTER_MAX_SYMBOL_LENGTH + 1];
uint8_t Syster::symbol_pos_;
bool Syster::is_active_;

// --- helpers ---
#define isSyster(k) (k == kaleidoscope::ranges::SYSTER)

// --- api ---
void Syster::reset(void) {
  symbol_pos_ = 0;
  symbol_[0] = 0;
  is_active_ = false;
}

bool Syster::is_active(void) {
  return is_active_;
}

// --- hooks ---
EventHandlerResult Syster::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t keyState) {
  if (!is_active_) {
    if (!isSyster(mapped_key))
      return EventHandlerResult::OK;

    if (keyToggledOn(keyState)) {
      is_active_ = true;
      systerAction(StartAction, NULL);
    }
    return EventHandlerResult::EVENT_CONSUMED;
  }

  if (keyState & INJECTED)
    return EventHandlerResult::OK;

  if (isSyster(mapped_key)) {
    return EventHandlerResult::EVENT_CONSUMED;
  }

  if (mapped_key == Key_Backspace && symbol_pos_ == 0) {
    return EventHandlerResult::EVENT_CONSUMED;
  }

  if (keyToggledOff(keyState)) {
    if (mapped_key == Key_Spacebar) {
      for (uint8_t i = 0; i <= symbol_pos_; i++) {
        handleKeyswitchEvent(Key_Backspace, UNKNOWN_KEYSWITCH_LOCATION, IS_PRESSED | INJECTED);
        hid::sendKeyboardReport();
        handleKeyswitchEvent(Key_Backspace, UNKNOWN_KEYSWITCH_LOCATION, WAS_PRESSED | INJECTED);
        hid::sendKeyboardReport();
      }

      systerAction(EndAction, NULL);

      symbol_[symbol_pos_] = 0;
      systerAction(SymbolAction, symbol_);
      reset();

      return EventHandlerResult::EVENT_CONSUMED;
    }
  }

  if (keyToggledOn(keyState)) {
    if (mapped_key == Key_Backspace) {
      if (symbol_pos_ > 0)
        symbol_pos_--;
    } else {
      const char c = keyToChar(mapped_key);
      if (c)
        symbol_[symbol_pos_++] = c;
    }
  }

  return EventHandlerResult::OK;
}

// Legacy V1 API

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
void Syster::begin() {
  Kaleidoscope.useEventHandlerHook(legacyEventHandler);
}

Key Syster::legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t key_state) {
  EventHandlerResult r = ::Syster.onKeyswitchEvent(mapped_key, row, col, key_state);
  if (r == EventHandlerResult::OK)
    return mapped_key;
  return Key_NoKey;
}
#endif

}

__attribute__((weak)) const char keyToChar(Key key) {
  if (key.flags != 0)
    return 0;

  switch (key.keyCode) {
  case Key_A.keyCode ... Key_Z.keyCode:
    return 'a' + (key.keyCode - Key_A.keyCode);
  case Key_1.keyCode ... Key_0.keyCode:
    return '1' + (key.keyCode - Key_1.keyCode);
  }

  return 0;
}

__attribute__((weak)) void systerAction(kaleidoscope::Syster::action_t action, const char *symbol) {
}

kaleidoscope::Syster Syster;
