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
/// \brief PUSCH demodulator interface.

#pragma once

#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/channel_processors/ulsch_placeholder_list.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/phy/upper/rb_allocation.h"

namespace srsgnb {

/// \brief PUSCH demodulator interface.
///
/// The demodulation of a PUSCH consists of:
/// - extracting allocated REs from the resource grid,
/// - equalizing of the extracted RE,
/// - soft-demodulation of the complex data, and
/// - descrambling.
class pusch_demodulator
{
public:
  /// Maximum number of REs per codeword in a single transmission.
  static constexpr unsigned MAX_CODEWORD_SIZE =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * pusch_constants::MAX_NOF_LAYERS;

  /// Maximum number of LLRs per codeword in a single transmission.
  static constexpr unsigned MAX_NOF_DATA_LLR = MAX_CODEWORD_SIZE * pusch_constants::MAX_MODULATION_ORDER;

  /// Parameters defining the demodulation procedure of a PUSCH transmission.
  struct configuration {
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.1.1.
    uint16_t rnti;
    /// Allocation RB list: the entries set to true are used for transmission.
    bounded_bitset<MAX_RB> rb_mask;
    /// Modulation scheme used for transmission.
    modulation_scheme modulation;
    /// Time domain allocation within a slot: start symbol index {0, ..., 12}.
    unsigned start_symbol_index;
    /// Time domain allocation within a slot: number of symbols {1, ..., 14}.
    unsigned nof_symbols;
    /// OFDM symbols containing DM-RS: boolean mask.
    std::array<bool, MAX_NSYMB_PER_SLOT> dmrs_symb_pos;
    /// DM-RS configuration type.
    dmrs_type dmrs_config_type;
    /// Number of DM-RS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.1.1. Range is {0, ..., 1023}.
    unsigned n_id;
    /// Number of transmit layers.
    unsigned nof_tx_layers;
    /// UL-SCH Scrambling placeholder list (See \ref ulsch_placeholder_list for more information).
    ulsch_placeholder_list placeholders;
    /// Receive antenna port indices the PUSCH transmission is mapped to.
    static_vector<uint8_t, MAX_PORTS> rx_ports;
  };

  /// Default destructor.
  virtual ~pusch_demodulator() = default;

  /// \brief Demodulates a PUSCH transmission.
  ///
  /// Computes log-likelihood ratios from channel samples by reversing all the operations described in TS38.211 Section
  /// 6.3.1.
  ///
  /// \remarks
  /// - The size of \c data determines the codeword size in soft bits.
  /// - The total number of LLR must be consistent with the number of RE allocated to the transmission.
  ///
  /// \param[out] codeword  Codeword soft bits destination buffer.
  /// \param[in]  grid      Resource grid for the current slot.
  /// \param[in]  estimates Channel estimates for the REs allocated to the PUSCH transmission.
  /// \param[in]  config    Configuration parameters.
  virtual void demodulate(span<log_likelihood_ratio>  codeword,
                          const resource_grid_reader& grid,
                          const channel_estimate&     estimates,
                          const configuration&        config) = 0;
};

} // namespace srsgnb
