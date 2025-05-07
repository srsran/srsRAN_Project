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

/// \file
/// \brief PUCCH detector interface for Formats 0 and 1.

#pragma once

#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_format1_map.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_uci_message.h"
#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_reader;

/// PUCCH detector interface for Formats 0 and 1.
class pucch_detector
{
public:
  /// Collects PUCCH Format 0 detector parameters.
  struct format0_configuration {
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix.
    cyclic_prefix cp;
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
    unsigned nof_harq_ack;
    /// Set to \c true if the PUCCH is used for reporting scheduling request.
    bool sr_opportunity;
    /// Port indexes used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Collects PUCCH Format 1 detector parameters.
  struct format1_configuration {
    /// Slot and numerology.
    slot_point slot;
    /// Cyclic prefix.
    cyclic_prefix cp;
    /// \brief PRB index used for the PUCCH transmission within the BWP {0, ..., 274}.
    ///
    /// Index of the PRB prior to frequency hopping or for no frequency hopping as per TS38.213 Section 9.2.1.
    unsigned starting_prb;
    /// \brief PRB index used for the PUCCH transmission within the BWP after frequency hopping {0, ..., 274}.
    ///
    /// Index of the PRB posterior to frequency hopping as per TS38.213 Section 9.2.1, if intra-slot frequency hopping
    /// is enabled, empty otherwise.
    std::optional<unsigned> second_hop_prb;
    /// Index of the first OFDM symbol allocated to the PUCCH {0, ..., 10}.
    unsigned start_symbol_index;
    /// Number of OFDM symbols allocated to the PUCCH {4, ..., 14}.
    unsigned nof_symbols;
    /// Group hopping scheme.
    pucch_group_hopping group_hopping;
    /// Port indexes used for the PUCCH reception.
    static_vector<uint8_t, MAX_PORTS> ports;
    /// Amplitude scaling factor.
    float beta_pucch;
    /// \brief Time-domain orthogonal cover code index {0, ..., 6}.
    ///
    /// Parameter \e timeDomainOCC in TS38.213 Section 9.2.1. Corresponds to the index \f$i\f$ of the time-domain
    /// orthogonal spreading sequence \f$w_i(m)\f$ in TS38.211 Section 6.3.2.4.1.
    unsigned time_domain_occ;
    /// \brief Cyclic shift initial index {0, ..., 11}.
    ///
    /// Index used to retrieve the cyclic shift for generating the low-PAPR sequence. Specifically, it corresponds to
    /// parameter \f$m_0\f$ in the formula for the cyclic shift \f$\alpha\f$ in TS38.211 Section 6.3.2.2.2.
    unsigned initial_cyclic_shift;
    /// \brief Pseudorandom generator initialization seed {0, ..., 1023}.
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
    unsigned nof_harq_ack;
  };

  /// Gathers the data obtained from detecting a PUCCH transmission.
  struct pucch_detection_result {
    /// UCI message conveyed by the PUCCH transmission.
    pucch_uci_message uci_message;
    /// Detection metric normalized with respect to the detection threshold.
    float detection_metric;
  };

  /// Pairs the detection result and CSI information of a PUCCH transmission.
  struct pucch_detection_result_csi {
    /// Detection result.
    pucch_detection_result detection_result;
    /// Channel state information.
    channel_state_information csi;
  };

  /// Default destructor.
  virtual ~pucch_detector() = default;

  /// \brief Detects a PUCCH Format 0 transmission.
  /// \param[in]  grid   Input resource grid.
  /// \param[in]  config PUCCH Format 0 configuration parameters.
  /// \return A pair containing the detected PUCCH message and the channel state information.
  virtual std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                         const format0_configuration& config) = 0;

  /// \brief Detects multiplexed PUCCH Format 1 transmissions.
  ///
  /// All multiplexed PUCCH transmissions share the configuration parameters in \c config, except for the initial cyclic
  /// shift, the time domain orthogonal cover code and the number of ACK bits, which are specified in \c mux_map.
  /// \param[in]  grid               Input resource grid.
  /// \param[in]  config             PUCCH Format 1 common configuration parameters (parameters \c initial_cyclic_shift,
  ///                                \c time_domain_occ and \c nof_harq_ack are ignored).
  /// \param[in]  mux_nof_harq_ack   Multiplexed PUCCHs - each (initial cyclic shift, time domain OCC) pair is mapped to
  ///                                the number of HARQ-ACK bits, if the corresponding PUCCH is scheduled, or to
  ///                                \c nullopt otherwise.
  /// \return A reference to a map of results - each (initial cyclic shift, time domain OCC) pair is mapped to the
  ///         corresponding detection result, if the PUCCH is scheduled, or to \c nullopt otherwise.
  virtual const pucch_format1_map<pucch_detection_result_csi>&
  detect(const resource_grid_reader&        grid,
         const format1_configuration&       config,
         const pucch_format1_map<unsigned>& mux_nof_harq_ack) = 0;
};
} // namespace srsran
