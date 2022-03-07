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
  /// Current numerology for FR1: (0,1,2) and FR2: (2,3)
  unsigned numerology;
  /// System frame number (0...1023)
  unsigned frame;
  /// Subframe index (0...9)
  unsigned subframe;
  /// Slot index (0...79), range depends on numerology
  unsigned slot;

  /// \brief Get the subframe number in the system frame with a 10240 millisecond period
  /// \return The system subframe index
  unsigned get_system_subframe() const { return frame * NSUBFRAMES_PER_FRAME + subframe; }

  /// \brief Get the slot number in the system frame with a 10240 millisecond period
  /// \return The system slot index
  unsigned get_system_slot() const { return slot + (get_system_subframe() << numerology); }

  /// \brief Get the half radio frame flag
  /// \return true if the slot context belongs to the second half of the entire radio frame, otherwise false
  bool get_half_radio_frame() const { return subframe >= (NSUBFRAMES_PER_FRAME / 2); }

  /// \brief Get the slot index in the radio frame
  /// \return the slot index within a full frame (0...799), range depends on numerology. For 15 kHz SCS the range is
  /// (0...10).
  unsigned get_frame_slot() const { return (subframe << numerology) + slot; }

  /// \brief Get the slot index in half frame
  /// \return the slot index within a half frame (0...399), range depends on numerology. For 15 kHz SCS the range is
  /// (0...4).
  unsigned get_half_frame_slot() const { return ((subframe % NSUBFRAMES_PER_HALF_FRAME) << numerology) + slot; }

  /// \brief Overrides the equal operator for two slot contexts
  /// \param [in] other Provides the reference to other slot context
  /// \return true if the slots numerology, frame, subframe and slot are equal, otherwise false
  bool operator==(const slot_context_t& other) const
  {
    return (numerology == other.numerology) && (frame == other.frame) && (subframe == other.subframe) &&
           (slot == other.slot);
  }
};

} // namespace srsgnb

#endif // SRSGNB_ADT_SLOT_CONTEXT_H