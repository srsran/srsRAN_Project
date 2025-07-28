/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/expected.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_format1_map.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor_result.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pucch/pucch_context.h"
#include "srsran/ran/slot_point.h"
#include <string>

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
    /// Context information.
    std::optional<pucch_context> context;
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix.
    cyclic_prefix cp;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// \brief PRB index used for the PUCCH transmission within the BWP {0, ..., 274}.
    ///
    /// Index of the PRB prior to frequency hopping or for no frequency hopping as per TS38.213 Section 9.2.1.
    unsigned starting_prb;
    /// \brief PRB index used for the PUCCH transmission within the BWP after frequency hopping {0, ..., 274}.
    ///
    /// Index of the PRB posterior to frequency hopping as per TS38.213 Section 9.2.1, if intra-slot frequency hopping
    /// is enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// Index of the first OFDM symbol allocated to the PUCCH {0, ..., 13}.
    unsigned start_symbol_index;
    /// Number of OFDM symbols allocated to the PUCCH {1, 2}.
    unsigned nof_symbols;
    /// \brief Cyclic shift initial index {0, ..., 11}.
    ///
    /// Index used to retrieve the cyclic shift for generating the low-PAPR sequence. Specifically, it corresponds to
    /// parameter \f$m_0\f$ in the formula for the cyclic shift \f$\alpha\f$ in TS38.211 Section 6.3.2.2.2.
    unsigned initial_cyclic_shift;
    /// \brief Sequence hopping identifier {0, ..., 1023}.
    ///
    /// Corresponds to parameter \f$n_{\textup{ID}}\f$ in TS38.211 Section 6.3.2.2.1.
    ///
    /// It must be set to the higher layer parameter \e hoppingID (see TS38.331 Section 6.3.2 &mdash; Information
    /// Element \e PUCCH-ConfigCommon) if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id;
    /// \brief Number of expected HARQ-ACK bits {0, 1, 2}.
    ///
    /// This parameter should be set to zero when trying to detect a positive scheduling request only.
    uint16_t nof_harq_ack;
    /// Set to \c true if the PUCCH is used for reporting scheduling request.
    bool sr_opportunity;
    /// Port indices used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Collects PUCCH Format 1 parameters.
  struct format1_configuration {
    /// Context information.
    std::optional<pucch_context> context;
    /// Slot and numerology.
    slot_point slot;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// \brief PRB index used for the PUCCH transmission within the BWP {0, ..., 274}.
    ///
    /// Index of the PRB prior to frequency hopping or for no frequency hopping as per TS38.213 Section 9.2.1.
    unsigned starting_prb;
    /// \brief Index of the PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Index of the PRB used for the second hop of the PUCCH transmission within the BWP {0, ..., 274} if intra-slot
    /// frequency hopping is enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// \brief Parameter \f$n_{\textup{ID}}\f$ in TS38.211 Section 6.3.2.2.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e hoppingID given by TS38.331 Section 6.3.2, Information Element
    /// \e PUCCH-ConfigCommon, if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id;
    /// Number of expected HARQ-ACK bits {0, 1, 2} (see also \ref PUCCH_payload_size "here").
    uint16_t nof_harq_ack;
    /// Port indices used for the PUCCH reception.
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

  /// Collects common PUCCH Format 1 parameters.
  struct format1_common_configuration {
    /// Slot and numerology.
    slot_point slot;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// \brief PRB index used for the PUCCH transmission within the BWP {0, ..., 274}.
    ///
    /// Index of the PRB prior to frequency hopping or for no frequency hopping as per TS38.213 Section 9.2.1.
    unsigned starting_prb;
    /// \brief Index of the PRB after frequency hopping as per TS38.213 Section 9.2.1.
    ///
    /// Index of the PRB used for the second hop of the PUCCH transmission within the BWP {0, ..., 274} if intra-slot
    /// frequency hopping is enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// \brief Parameter \f$n_{\textup{ID}}\f$ in TS38.211 Section 6.3.2.2.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e hoppingID given by TS38.331 Section 6.3.2, Information Element
    /// \e PUCCH-ConfigCommon, if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id;
    /// Port indices used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> ports;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Start symbol index {0, ..., 10}.
    unsigned start_symbol_index;

    /// Default constructor.
    format1_common_configuration() = default;

    /// Construct the configuration from a complete Format 1 configuration parameters.
    explicit format1_common_configuration(const format1_configuration& config) :
      slot(config.slot),
      bwp_size_rb(config.bwp_size_rb),
      bwp_start_rb(config.bwp_start_rb),
      cp(config.cp),
      starting_prb(config.starting_prb),
      second_hop_prb(config.second_hop_prb),
      n_id(config.n_id),
      ports(config.ports),
      nof_symbols(config.nof_symbols),
      start_symbol_index(config.start_symbol_index)
    {
    }

    /// Determines whether the Format 1 common configuration is equal to another.
    bool operator==(const format1_common_configuration& other) const
    {
      return (other.slot == slot) && (other.bwp_size_rb == bwp_size_rb) && (other.bwp_start_rb == bwp_start_rb) &&
             (cp == other.cp) && (starting_prb == other.starting_prb) && (second_hop_prb == other.second_hop_prb) &&
             (n_id == other.n_id) && (ports == other.ports) && (nof_symbols == other.nof_symbols) &&
             (start_symbol_index == other.start_symbol_index);
    }
  };

  /// Collects PUCCH Format 1 batch parameters.
  struct format1_batch_configuration {
    /// Collects UE dedicated entries.
    struct ue_dedicated_entry {
      /// Context information.
      std::optional<pucch_context> context;
      /// Number of expected HARQ-ACK bits {0, 1, 2} (see also \ref PUCCH_payload_size "here").
      uint16_t nof_harq_ack;
    };

    /// Default constructor.
    format1_batch_configuration() = default;

    /// Construct the configuration from a complete Format 1 configuration parameters.
    explicit format1_batch_configuration(const format1_configuration& config) : common_config(config)
    {
      entries.insert(config.initial_cyclic_shift,
                     config.time_domain_occ,
                     {.context = config.context, .nof_harq_ack = config.nof_harq_ack});
    }

    /// Common UE configuration.
    format1_common_configuration common_config;
    /// UE entry map.
    pucch_format1_map<ue_dedicated_entry> entries;
  };

  /// Collects PUCCH Format 2 parameters.
  struct format2_configuration {
    /// Context information.
    std::optional<pucch_context> context;
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Port indices used for the PUCCH reception.
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
    std::optional<unsigned> second_hop_prb;
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
    uint16_t nof_harq_ack;
    /// Number of expected SR bits {0, ..., 4}.
    unsigned nof_sr;
    /// Number of expected CSI Part 1 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part1;
    /// Number of expected CSI Part 2 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part2;
  };

  /// Collects specific PUCCH Format 3 parameters.
  struct format3_configuration {
    /// Context information.
    std::optional<pucch_context> context;
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Port indices used for the PUCCH reception.
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
    std::optional<unsigned> second_hop_prb;
    /// \brief Number of PRB {1, ..., 16}.
    ///
    /// This parameter is equivalent to parameter \f$N^\textup{PUCCH, 2}_\textup{PRB}\f$ in TS38.212 Section 6.3.1.4,
    /// and parameter \f$M^\textup{PUCCH}_\textup{RB,min}\f$ in TS38.213 Section 9.2.5.2.
    unsigned nof_prb;
    /// Start symbol index {0, ..., 10}.
    unsigned start_symbol_index;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.2.5.1.
    uint16_t rnti;
    /// \brief Parameter \f$n_{\textup{ID}}\f$ in TS38.211 Section 6.3.2.2.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e hoppingID given by TS38.331 Section 6.3.2, Information Element
    /// \e PUCCH-ConfigCommon, if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id_hopping;
    /// \brief Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.5.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e dataScramblingIdentityPUSCH if configured. Otherwise, it must
    /// be equal to the physical cell identifier \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id_scrambling;
    /// Number of expected HARQ-ACK bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    uint16_t nof_harq_ack;
    /// Number of expected SR bits {0, ..., 4}.
    unsigned nof_sr;
    /// Number of expected CSI Part 1 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part1;
    /// Number of expected CSI Part 2 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part2;
    /// Set to higher layer parameter \e additionalDMRS described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool additional_dmrs;
    /// Set to higher layer parameter \e pi2BPSK described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool pi2_bpsk;
  };

  /// Collects specific PUCCH Format 4 parameters.
  struct format4_configuration {
    /// Context information.
    std::optional<pucch_context> context;
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix configuration for the slot.
    cyclic_prefix cp;
    /// Port indices used for the PUCCH reception.
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
    std::optional<unsigned> second_hop_prb;
    /// Start symbol index {0, ..., 10}.
    unsigned start_symbol_index;
    /// Number of symbols for the PUCCH transmission {4, ..., 14}.
    unsigned nof_symbols;
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.2.5.1.
    uint16_t rnti;
    /// \brief Parameter \f$n_{\textup{ID}}\f$ in TS38.211 Section 6.3.2.2.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e hoppingID given by TS38.331 Section 6.3.2, Information Element
    /// \e PUCCH-ConfigCommon, if it is configured. Otherwise, it must be equal to the physical cell identifier
    /// \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id_hopping;
    /// \brief Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.5.1 {0, ..., 1023}.
    ///
    /// It must be set to the higher layer parameter \e dataScramblingIdentityPUSCH if configured. Otherwise, it must
    /// be equal to the physical cell identifier \f$N_{\textup{ID}}^{\textup{cell}}\f$.
    unsigned n_id_scrambling;
    /// Number of expected HARQ-ACK bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    uint16_t nof_harq_ack;
    /// Number of expected SR bits {0, ..., 4}.
    unsigned nof_sr;
    /// Number of expected CSI Part 1 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part1;
    /// Number of expected CSI Part 2 bits {0, ..., 1706} (see also \ref PUCCH_payload_size "here").
    unsigned nof_csi_part2;
    /// Set to higher layer parameter \e additionalDMRS described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool additional_dmrs;
    /// Set to higher layer parameter \e pi2BPSK described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-FormatConfig.
    bool pi2_bpsk;
    /// Set to higher layer parameter \e occ-Index described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-format4.
    unsigned occ_index;
    /// Set to higher layer parameter \e occ-Length described in TS38.331 Section 6.3.2, Information Element \e
    /// PUCCH-format4 {2, 4}.
    unsigned occ_length;
  };

  /// Default destructor.
  virtual ~pucch_processor() = default;

  /// \brief Processes a PUCCH Format 0 message.
  /// \param[in] grid    Resource grid.
  /// \param[in] config  PUCCH Format 0 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) = 0;

  /// \brief Processes a batch of PUCCH Format 1 configurations.
  /// \param[in] grid   Resource grid.
  /// \param[in] config PUCCH Format 1 batch configuration.
  /// \return A map the PUCCH Format 1 detection results.
  virtual pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                            const format1_batch_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 2 message.
  /// \param[in] grid    Resource grid.
  /// \param[in] config  PUCCH Format 2 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 3 message.
  /// \param[in] grid    Resource grid.
  /// \param[in] config  PUCCH Format 3 configuration.
  /// \return The PUCCH process result.
  virtual pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) = 0;

  /// \brief Processes a PUCCH Format 4 message.
  /// \param[in] grid    Resource grid.
  /// \param[in] config  PUCCH Format 4 configuration.
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
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 1 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 2 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 3 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const = 0;

  /// \brief Validates PUCCH Format 4 configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const = 0;
};

} // namespace srsran
