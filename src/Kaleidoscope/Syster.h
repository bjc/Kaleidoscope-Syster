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

#pragma once

#include <Kaleidoscope.h>
#include <Kaleidoscope-Ranges.h>

#define SYSTER_MAX_SYMBOL_LENGTH 32

#define SYSTER ((Key) { .raw = kaleidoscope::ranges::SYSTER })

namespace kaleidoscope {

class Syster : public kaleidoscope::Plugin {
 public:
  typedef enum {
    StartAction,
    EndAction,
    SymbolAction
  } action_t;

  Syster(void) {}

  static void reset(void);

  bool is_active(void);

  EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t keyState);

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
  kaleidoscope::EventHandlerResult onSetup() {
    return kaleidoscope::EventHandlerResult::OK;
  }

 protected:
  void begin();
  static Key legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t key_state);
#endif

 private:
  static char symbol_[SYSTER_MAX_SYMBOL_LENGTH + 1];
  static uint8_t symbol_pos_;
  static bool is_active_;
};
};

const char keyToChar(Key key);
void systerAction(kaleidoscope::Syster::action_t action, const char *symbol);

extern kaleidoscope::Syster Syster;
