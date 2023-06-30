/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/pdcch/pdcch_context.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_mapper;

/// \brief Describes the PDCCH processor interface.
///
/// The PDCCH processor shall:
/// - encode DCI, as per TS38.212 Section 7.3, where the DCI payload is given in \c dci_description::payload,
/// - modulate PDCCH, as per TS38.211 Section 7.3.2, and
/// - generate DMRS for PDCCH, as per TS38.211 Section 7.4.1.3.
///
/// \remark One Resource Element Group (REG) is equivalent to one Resource Block (RB), that is 12 Resource Elements
/// (REs).
/// \remark One Control-Channel Element (CCE) is equivalent to 6 REGs.
/// \remark REGs are indexed first in time (for each symbol) and then in frequency.
class pdcch_processor
{
public:
  /// Describes a DCI transmission.
  struct dci_description {
    /// Parameter \f$x_{rnti, k}\f$ as per TS38.211 section 7.3.2 {0...65535}.
    unsigned rnti;
    /// Parameter \f$n_{ID}\f$ used for DMRS scrambling as per TS38.211 Section 7.4.1.3.1 {0...65535}.
    unsigned n_id_pdcch_dmrs;
    /// Parameter \f$n_{ID}\f$ used in data scrambling as per TS38.211 Section 7.3.2.3 {0...65535}.
    unsigned n_id_pdcch_data;
    /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling in TS38.211 Section 7.3.2.3 {1...65535}.
    unsigned n_rnti;
    /// CCE start index used to send the DCI {0, 135}.
    unsigned cce_index;
    /// Indicates the number of CCE used by the PDCCH transmission as per TS38.211 Section 7.3.2.1 {1, 2, 4, 8, 16}.
    unsigned aggregation_level;
    /// Ratio of PDCCH DM-RS EPRE to SSS EPRE in decibels.
    float dmrs_power_offset_dB;
    /// Ratio of PDCCH Data EPRE to SSS EPRE in decibels.
    float data_power_offset_dB;
    /// DCI payload as unpacked bits.
    static_vector<uint8_t, pdcch_constants::MAX_DCI_PAYLOAD_SIZE> payload;
    /// Precoding configuration.
    precoding_configuration precoding;
  };

  /// CCE-to-REG mapping types as per TS38.211 Section 7.3.2.2.
  enum class cce_to_reg_mapping_type {
    /// CORESET is configured by the PBCH or SIB1, (subcarrier 0 of the CORESET)
    CORESET0 = 0,
    /// Not configured by PBCH or SIB 1, and non-interleaved.
    NON_INTERLEAVED,
    /// Not configured by PBCH or SIB 1, and interleaved.
    INTERLEAVED
  };

  /// \brief Describes CORESET parameters.
  ///
  /// Provides the Control Resource Set (CORESET, TS38.331 \c ControlResourceSet for more information) related
  /// parameters common for up to \c MAX_NOF_DCI Downlink Control Information (DCI) transmissions.
  ///
  /// For  \c controlResourceSetZero (CORESET 0, TS38.213 Section 13 for more information) there are some exceptions:
  /// - \c cce_to_reg_mapping_type shall be set to \c CORESET0.
  /// - \c bwp_start_rb and \c bwp_size_rb indicate the CORESET start point and contiguous resource blocks.
  /// - \c shift_index shall be equal to the physical cell identifier {0...1007}.
  /// - \c cce_to_reg_mapping_type, \c frequency_resources, \c reg_bundle_size, \c interleaver_size are ignored.
  struct coreset_description {
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// Starting OFDM symbol for the CORESET.
    unsigned start_symbol_index;
    /// Contiguous time duration of the CORESET in number of symbols {1,2,3}. Corresponds to L1 parameter
    /// \f$N^{CORESET}_{symb}\f$ as per RS 38.211 Section 7.3.2.2.
    unsigned duration;
    /// \brief Frequency domain resources.
    ///
    /// This is a bitmap defining non-overlapping groups of 6 PRBs in ascending order. Defined as \f$N_{CORESET}^RB\f$
    /// as per TS38.211 Section 7.3.2.2.
    freq_resource_bitmap frequency_resources;
    /// CCE-to-REG mapping.
    cce_to_reg_mapping_type cce_to_reg_mapping;
    /// \brief The number of REGs in a bundle. Corresponds to parameter \f$L\f$ in TS38.211 7.3.2.2.
    ///
    /// Ignored for \c cce_to_reg_mapping_type set to \c INTERLEAVED or \c CORESET0. It must be:
    /// - {2,6} for duration of 1 or 2 symbols, and
    /// - {3,6} for duration of 3 symbols.
    unsigned reg_bundle_size;
    /// \brief The CCE-to-REG interleaver size. Corresponds to parameter \f$R\f$ in TS38.211 7.3.2.2.
    ///
    /// Ignored for \c cce_to_reg_mapping_type set to \c INTERLEAVED or \c CORESET0. It must be {2,3,6}.
    unsigned interleaver_size;
    /// \brief Shift index \f$n_{shift}\f$ in TS38.211 Section 7.3.2.2.
    ///
    /// The value is:
    /// - set to the physical cell identifier for a transmission in CORESET0,
    /// - ignored for non-interleaved mapping, and
    /// - set to {0,275} for non-interleaved CCE-to-REG mapping.
    unsigned shift_index;
  };

  /// Collects the PDCCH parameters for a transmission.
  struct pdu_t {
    /// Context information.
    optional<pdcch_context> context;
    /// Indicates the slot and numerology.
    slot_point slot;
    /// Cyclic prefix type.
    cyclic_prefix cp;
    /// Provides CORESET description.
    coreset_description coreset;
    /// Downlink Control Information.
    dci_description dci;
  };

  /// Default detsructor.
  virtual ~pdcch_processor() = default;

  /// \brief Processes a PDCCH transmission.
  ///
  /// \param[out] mapper Resource grid mapper interface.
  /// \param[in] pdu     Necessary parameters to process the PDCCH transmission.
  virtual void process(resource_grid_mapper& mapper, const pdu_t& pdu) = 0;
};

/// \brief Describes the PDCCH processor validator interface.
class pdcch_pdu_validator
{
public:
  /// Default destructor.
  virtual ~pdcch_pdu_validator() = default;

  /// \brief Validates PDCCH processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const pdcch_processor::pdu_t& pdu) const = 0;
};

} // namespace srsran
