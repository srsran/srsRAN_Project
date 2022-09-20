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

#include "pucch_processor_result.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_processors/pucch_uci_message.h"

namespace srsgnb {

class pucch_processor
{
public:
  /// \brief Collects common PUCCH configuration parameters among the different formats.
  ///
  /// \remark The total payload size for Format 0 and 1 shall not exceed two.
  /// \remark The total payload size for Formats 2, 3 and 4 shall not exceed 1706.
  struct common_configuration {
    /// Slot and numerology.
    slot_point slot;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// \brief Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274}.
    ///
    /// Index of the first PRB prior to frequency hopping or for no frequency hopping as per TS38.213 Section 9.2.1.
    unsigned starting_prb;
    /// \brief Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274} if it has a value.
    /// Otherwise, intra-slot frequency is disabled.
    ///
    /// Index of the first PRB after frequency hopping as per TS38.213 Section 9.2.1.
    optional<unsigned> second_hop_prb;
    /// Number of expected SR bits {0, ..., 4}.
    unsigned nof_sr;
    /// Number of expected HARQ-ACK bits {0, ..., 1706}.
    unsigned nof_harq_ack;
    /// Number of expected CSI part 1 bits {0, ..., 1706}.
    unsigned nof_csi_part1;
    /// Number of expected CSI part 2 bits {0, ..., 1706}.
    unsigned nof_csi_part2;
  };

  /// Collects specific PUCCH Format 0 parameters.
  struct format0_configuration {
    /// Common parameters.
    common_configuration common;
    /// Initial cyclic shift {0, ..., 11}.
    unsigned initial_cyclic_shift;
    /// Number of symbols for the PUCCH transmission {1, 2}.
    unsigned nof_symbols;
    /// Start symbol index {0, ..., 13}.
    unsigned start_symbol_index;
  };

  /// Collects specific PUCCH Format 1 parameters.
  struct format1_configuration {
    /// Common parameters.
    common_configuration common;
    /// Initial cyclic shift {0, ..., 11}.
    unsigned initial_cyclic_shift;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Start symbol index {0, ..., 11}.
    unsigned start_symbol_index;
    /// Time domain orthogonal cyclic code {0, ..., 6}.
    unsigned time_domain_occ;
  };

  /// Collects specific PUCCH Format 2 parameters.
  struct format2_configuration {
    /// Common parameters.
    common_configuration common;
    /// Number of PRB {1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 16}.
    unsigned nof_prb;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Start symbol index {0, ..., 10}.
    unsigned start_symbol_index;
  };

  /// Collects specific PUCCH Format 3 parameters.
  struct format3_configuration {
    /// Common parameters.
    common_configuration common;
  };

  /// Collects specific PUCCH Format 4 parameters.
  struct format4_configuration {
    /// Common parameters.
    common_configuration common;
  };

  /// Default destructor.
  virtual ~pucch_processor() = default;

  /// \brief Processes a PUCCH Format 0 message.
  /// \param[out] grid    Resource grid.
  /// \param[in]  config  PUCCH Format 0 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 1 message.
  /// \param[out] grid    Resource grid.
  /// \param[in]  config  PUCCH Format 1 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format1_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 2 message.
  /// \param[out] grid    Resource grid.
  /// \param[in]  config  PUCCH Format 2 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 3 message.
  /// \param[out] grid    Resource grid.
  /// \param[in]  config  PUCCH Format 3 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 4 message.
  /// \param[out] indication PUCCH process result.
  /// \param[in]  config     PUCCH Format 4 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) = 0;
};

} // namespace srsgnb
