/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_ADT_SLOT_CONTEXT_H
#define SRSGNB_ADT_SLOT_CONTEXT_H

namespace srsgnb {

/// Describes a slot context
struct slot_context_t {
  unsigned numerology; /// Current numerology for FR1: (0,1,2) and FR2: (2,3)
  unsigned frame;      /// Frame index (0...1023)
  unsigned subframe;   /// Frame index (0...9)
  unsigned slot;       /// Slot index (0...79), range depends on numerology
};

slot_context_t operator+(const slot_context_t& x, unsigned n);

} // namespace srsgnb

#endif // SRSGNB_ADT_SLOT_CONTEXT_H