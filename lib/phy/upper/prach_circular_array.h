/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/upper/channel_processors/prach_detector.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// \brief Container for PRACH slot configurations.
///
/// PRACH slot configurations are stored in a circular array, indexed by slot.
class prach_circular_array
{
  /// Maximum number of PRACH configurations per slot.
  static constexpr unsigned MAX_NUM_PRACH_CONFIGS_PER_SLOT = 1U;
  /// Maximum number of slots supported by this container. This number should be big enough so an entry will not get
  /// reused until it gets processed.
  static constexpr unsigned MAX_NUM_SLOTS = 60U;

public:
  using prach_entry = static_vector<prach_detector::slot_configuration, MAX_NUM_PRACH_CONFIGS_PER_SLOT>;

  /// \brief Inserts the given configuration in the circular array using the information provided in the given context.
  ///
  /// \param[in] context Contains the information that will be used to index the configuration in the circular array.
  /// \param[in] config  Configuration stored in the circular array.
  void insert(const prach_buffer_context& context, const prach_detector::slot_configuration& config)
  {
    ring[context.slot.system_slot()].push_back(config);
  }

  /// \brief Returns the PRACH configuration entry associated with the given slot.
  ///
  /// \param[in] slot Slot to get the PRACH entry.
  /// \return The prach_entry associated with the given slot.
  prach_entry&       operator[](slot_point slot) { return ring[slot.system_slot()]; }
  const prach_entry& operator[](slot_point slot) const { return ring[slot.system_slot()]; }

private:
  /// Actual circular buffer of PRACH configurations.
  circular_array<prach_entry, MAX_NUM_SLOTS> ring;
};

} // namespace srsgnb
