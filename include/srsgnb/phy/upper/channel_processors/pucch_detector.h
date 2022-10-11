/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUCCH detector interface for Formats 0 and 1.

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/channel_processors/pucch_uci_message.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// PUCCH detector interface for Formats 0 and 1.
class pucch_detector
{
public:
  /// Collects PUCCH Format 0 detector parameters.
  struct format0_configuration {
    // Add here PUCCH demodulator parameters...
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
    optional<unsigned> second_hop_prb;
    /// Index of the first OFDM symbol allocated to the PUCCH {0, ..., 10}.
    unsigned start_symbol_index;
    /// Number of OFDM symbols allocated to the PUCCH {4, ..., 14}.
    unsigned nof_symbols;
    /// Antenna port the PUCCH is received at.
    unsigned port;
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
    /// Number of expected SR bits {0, 1}.
    unsigned nof_sr;
    /// Number of expected HARQ-ACK bits {0, 1, 2}.
    unsigned nof_harq_ack;
  };

  /// Default destructor.
  virtual ~pucch_detector() = default;

  /// \brief Detects a PUCCH Format 0 transmission.
  /// \param[in]  grid   Input resource grid.
  /// \param[in]  config PUCCH Format 0 configuration parameters.
  /// \return A pair containing the detected PUCCH message and the channel state information.
  virtual std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                         const format0_configuration& config) = 0;

  /// \brief Detects a PUCCH Format 1 transmission.
  ///
  /// Reverts the operations described in TS38.211 Section 6.3.2.4. See also TS38.213 Section 9.2 for more information
  /// about the configuration parameters and the format of the output UCI message. In particular, note that no SR bit is
  /// transmitted if HARQ-ACK bits are and that the UE does not transmit anything (DTX) if the UCI message only consists
  /// of a 0-valued SR bit.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Estimated channel.
  /// \param[in]  config    PUCCH Format 1 configuration parameters.
  /// \return The detected PUCCH message.
  virtual pucch_uci_message
  detect(const resource_grid_reader& grid, const channel_estimate& estimates, const format1_configuration& config) = 0;
};
} // namespace srsgnb
