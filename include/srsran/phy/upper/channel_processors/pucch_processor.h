/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/channel_processors/pucch_processor_result.h"
#include "srsran/phy/upper/channel_processors/pucch_uci_message.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/modulation_scheme.h"
#include "srsran/ran/pucch/pucch_context.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_reader;

/// \brief PUCCH processor interface for all formats.
///
/// \remark \anchor PUCCH_payload_size The total payload size for Formats 2, 3 and 4 shall not exceed 1706.
/// Specifically, one should have <tt>nof_sr + nof_harq_ack + nof_csi_part1 + nof_csi_part2 <= 1706</tt>.
class pucch_processor
{
public:
  /// Collects specific PUCCH Format 0 parameters.
  struct format0_configuration {
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Initial cyclic shift {0, ..., 11}.
    unsigned initial_cyclic_shift;
    /// Number of symbols for the PUCCH transmission {1, 2}.
    unsigned nof_symbols;
    /// Start symbol index {0, ..., 13}.
    unsigned start_symbol_index;
    /// Slot and numerology, for logging.
    slot_point slot;
  };

  /// Collects PUCCH Format 1 parameters.
  struct format1_configuration {
    /// Context information.
    optional<pucch_context> context;
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
    /// \brief Index of the first PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274} if intra-slot frequency hopping is
    /// enabled, empty otherwise.
    optional<unsigned> second_hop_prb;
    /// \brief Parameter \f$n_{\textup{ID}}\f$ in TS38.211 Section 6.3.2.2.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e hopingID given by TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-ConfigCommon, if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id;
    /// Number of expected HARQ-ACK bits {0, 1, 2} (see also \ref PUCCH_payload_size "here").
    unsigned nof_harq_ack;
    /// Port indexes used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> ports;
    /// Initial cyclic shift {0, ..., 11}.
    unsigned initial_cyclic_shift;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Start symbol index {0, ..., 10}.
    unsigned start_symbol_index;
    /// Time domain orthogonal cyclic code {0, ..., 6}.
    unsigned time_domain_occ;
  };

  /// Collects PUCCH Format 2 parameters.
  struct format2_configuration {
    /// Context information.
    optional<pucch_context> context;
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Port indexes used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> ports;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// \brief Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274}.
    ///
    /// Index of the first PRB prior to frequency hopping or for no frequency hopping as per TS38.213 Section 9.2.1.
    unsigned starting_prb;
    /// \brief Index of the first PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Lowest PRB index used for the PUCCH transmission within the BWP {0, ..., 274} if intra-slot frequency hopping is
    /// enabled, empty otherwise.
    optional<unsigned> second_hop_prb;
    /// \brief Number of PRB {1, ..., 16}.
    ///
    /// This parameter is equivalent to parameter \f$N^\textup{PUCCH, 2}_\textup{PRB}\f$ in TS38.212 Section 6.3.1.4,
    /// and parameter \f$M^\textup{PUCCH}_\textup{RB,min}\f$ in TS38.213 Section 9.2.5.2.
    unsigned nof_prb;
    /// Start symbol index {0, ..., 12}.
    unsigned start_symbol_index;
    /// Number of symbols for the PUCCH transmission {1, 2}.
    unsigned nof_symbols;
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.2.5.1.
    uint16_t rnti;
    /// Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.5.1. Range is {0, ..., 1023}.
    unsigned n_id;
    /// \brief DM-RS scrambling identity {0, ..., 65535}.
    ///
    /// Corresponds to parameter \f$N_{\textup{ID}}^0\f$ in TS38.211 Section 6.4.1.3.2.1.
    ///
    /// It must be set to the higher layer parameter \e scramblingID0 given by TS38.331 Section 6.3.2, Information
    /// Element \e DMRS-UplinkConfig, if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id_0;
    /// Number of expected HARQ-ACK bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_harq_ack;
    /// Number of expected SR bits {0, ..., 4}.
    unsigned nof_sr;
    /// Number of expected CSI Part 1 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part1;
    /// Number of expected CSI Part 2 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part2;
  };

  /// Collects specific PUCCH Format 3 parameters.
  struct format3_configuration {
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Slot and numerology, for logging.
    slot_point slot;
  };

  /// Collects specific PUCCH Format 4 parameters.
  struct format4_configuration {
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Slot and numerology, for logging.
    slot_point slot;
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

/// \brief Describes the PUCCH processor validator interface.
class pucch_pdu_validator
{
public:
  /// Default destructor.
  virtual ~pucch_pdu_validator() = default;

  /// \brief Validates PUCCH Format 0 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format0_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 1 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format1_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 2 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format2_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 3 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format3_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 4 configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const pucch_processor::format4_configuration& config) const = 0;
};

} // namespace srsran
