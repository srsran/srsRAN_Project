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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/ran/pusch/pusch_constants.h"
#include "srsgnb/ran/uci/uci_constants.h"

namespace srsgnb {

/// \brief PUSCH demodulator interface.
///
/// The demodulation of a PUSCH consists of:
/// - extracting allocated REs from the resource grid,
/// - equalizing of the extracted RE,
/// - soft-demodulation of the complex data,
/// - descrambling, and
/// - demultiplexing data and control.
class pusch_demodulator
{
public:
  /// Maximum number of REs per codeword in a single transmission.
  static constexpr unsigned MAX_CODEWORD_SIZE = MAX_RB * NRE * MAX_NSYMB_PER_SLOT * MAX_PORTS / 2;

  /// Maximum number of LLRs per codeword in a single transmission.
  static constexpr unsigned MAX_NOF_DATA_LLR = MAX_CODEWORD_SIZE * pusch_constants::MAX_MODULATION_ORDER;

  /// Maximum UCI scaling \f$\alpha\f$ as per TS38.331 UCI-OnPUSCH.
  static constexpr unsigned UCI_ON_PUSCH_MAX_ALPHA = 1;

  /// \brief Maximum number of LLRs for HARQ-ACK in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1, \f$Q_{ACK}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_HARQ_ACK_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLRs for CSI Part1 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-1}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_CSI_PART1_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLRs for CSI Part2 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-2}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_CSI_PART2_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// Data LLR buffer type.
  using data_llr_buffer = static_vector<log_likelihood_ratio, MAX_NOF_DATA_LLR>;

  /// HARQ-ACK LLR buffer type.
  using harq_ack_llr_buffer = static_vector<log_likelihood_ratio, MAX_NOF_HARQ_ACK_LLR>;

  /// CSI Part1 LLR buffer type.
  using csi_part1_llr = static_vector<log_likelihood_ratio, MAX_NOF_CSI_PART1_LLR>;

  /// CSI Part2 LLR buffer type.
  using csi_part2_llr = static_vector<log_likelihood_ratio, MAX_NOF_CSI_PART2_LLR>;

  /// Parameters defining the demodulation procedure of a PUSCH transmission.
  struct configuration {
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.1.1.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP.
    unsigned bwp_size_rb;
    /// BWP start RB index from reference CRB.
    unsigned bwp_start_rb;
    /// Modulation scheme used for transmission.
    modulation_scheme modulation;
    /// Frequency domain allocation.
    rb_allocation freq_allocation;
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
    /// Scaling factor applied to the resource elements according to PUSCH power allocation in TS38.214 Section 6.1
    /// (see also TS38.213 Section 7.1).
    float scaling;
    /// Reserved RE pattern where PUSCH is not mapped.
    re_pattern_list reserved;
    /// Number of transmit layers.
    unsigned nof_tx_layers;
    /// Number of receive ports.
    unsigned nof_rx_ports;
  };

  /// Default destructor.
  virtual ~pusch_demodulator() = default;

  /// \brief Demodulates a PUSCH transmission.
  ///
  /// Demodulation reverses all the operations described in TS38.211 Section 6.3.1. The method also extracts control
  /// bits that were multiplexed with PUSCH data as per TS38.212 Section 6.2.7.
  ///
  /// \remarks
  /// - The size of \c data determines the codeword size in LLR.
  /// - The size of \c harq_ack determines the number of LLR for HARQ-ACK.
  /// - The size of \c csi_part1 determines the number of LLR for CSI Part1.
  /// - The size of \c csi_part2 determines the number of LLR for CSI Part2.
  /// - The total number of LLR must be consistent with the number of RE allocated to the transmission.
  ///
  /// \param[out] data      Codeword LLR destination buffer.
  /// \param[out] harq_ack  HARQ-ACK LLR destination buffer.
  /// \param[out] csi_part1 CSI Part1 LLR destination buffer.
  /// \param[out] csi_part2 CSI Part2 LLR destination buffer.
  /// \param[in]  grid      Resource grid for the current slot.
  /// \param[in]  estimates Channel estimates for the REs allocated to the PUSCH transmission.
  /// \param[in]  config    Configuration parameters.
  virtual void demodulate(data_llr_buffer&            data,
                          harq_ack_llr_buffer&        harq_ack,
                          csi_part1_llr&              csi_part1,
                          csi_part2_llr&              csi_part2,
                          const resource_grid_reader& grid,
                          const channel_estimate&     estimates,
                          const configuration&        config) = 0;
};

} // namespace srsgnb
