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

/// Defines the number of subframes in a frame
static constexpr unsigned NSUBFRAMES_PER_FRAME = 10;

/// Defines the number of subframes in half frame
static constexpr unsigned NSUBFRAMES_PER_HALF_FRAME = NSUBFRAMES_PER_FRAME / 2;

/// Describes a slot context
struct slot_context_t {
  unsigned numerology; /// Current numerology for FR1: (0,1,2) and FR2: (2,3)
  unsigned frame;      /// System frame number (0...1023)
  unsigned subframe;   /// Subframe index (0...9)
  unsigned slot;       /// Slot index (0...79), range depends on numerology

  /// Get the subframe number in the whole radio frame
  unsigned get_system_subframe() const { return frame * 10 + subframe; }

  /// Get the slot number in the whole radio frame
  unsigned get_system_slot() const { return slot + (get_system_subframe() << numerology); }

  /// Get the half radio frame flag
  bool get_half_radio_frame() const { return subframe >= (NSUBFRAMES_PER_FRAME / 2); }

  /// Get the slot index in the current radio frame
  unsigned get_frame_slot() const { return (subframe << numerology) + slot; }

  /// Get the slot index in half frame
  unsigned get_half_frame_slot() const { return ((subframe % NSUBFRAMES_PER_HALF_FRAME) << numerology) + slot; }
};

} // namespace srsgnb

#endif // SRSGNB_ADT_SLOT_CONTEXT_H