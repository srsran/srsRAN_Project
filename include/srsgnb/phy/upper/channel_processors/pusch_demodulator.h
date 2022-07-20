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

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimate.h"
#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/pusch/pusch_constants.h"
#include "srsgnb/ran/uci/uci_constants.h"

namespace srsgnb {

/// \brief Describes a PUSCH demodulator interface.
///
/// The PUSCH demodulation process:
/// - extracts the RE from the resource grid,
/// - equalizes of the extracted RE,
/// - soft-demodulates the complex data,
/// - undoes the scrambling sequences, and
/// - demultiplexes data and control.
class pusch_demodulator
{
public:
  /// Maximum number of RE per codeword in a single transmission.
  static constexpr unsigned MAX_CODEWORD_SIZE = MAX_RB * NRE * MAX_NSYMB_PER_SLOT * MAX_PORTS / 2;

  /// Maximum number of LLR per codeword in a single transmission.
  static constexpr unsigned MAX_NOF_DATA_LLR = MAX_CODEWORD_SIZE * pusch_constants::MAX_MODULATION_ORDER;

  /// Maximum UCI scaling \f$\alpha\f$ as per TS38.331 UCI-OnPUSCH.
  static constexpr unsigned UCI_ON_PUSCH_MAX_ALPHA = 1;

  /// \brief Maximum number of LLR for HARQ-ACK in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{ACK}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_HARQ_ACK_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLR for CSI Part1 in a single transmission.
  ///
  /// As per TS38.212 Section 6.3.2.4.1.1 in \f$Q_{CSI-1}'\f$ formula. Ceiling the value of \f$\sum
  /// ^{N_{symb,all}^{PUSCH}-1}_{l=l_0}M_{sc}^{UCI}(l)\f$ to the maximum number of resource elements in a PUSCH
  /// transmission.
  static constexpr unsigned MAX_NOF_CSI_PART1_LLR =
      MAX_RB * pusch_constants::MAX_NRE_PER_RB * UCI_ON_PUSCH_MAX_ALPHA * pusch_constants::MAX_MODULATION_ORDER;

  /// \brief Maximum number of LLR for CSI Part2 in a single transmission.
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

  /// Describes the necessary parameters to demodulate a PUSCH transmission.
  struct config_t {
    /// Provides \f$n_{RNTI}\f$ from TS38.211 section 6.3.1.1 Scrambling.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP.
    unsigned bwp_size_rb;
    /// BWP start RB index from reference CRB.
    unsigned bwp_start_rb;
    /// Modulation of codeword 1 (q = 0).
    modulation_scheme modulation1;
    /// Modulation of codeword 2 ( q = 1).
    modulation_scheme modulation2;
    /// Frequency domain allocation.
    rb_allocation freq_allocation;
    /// Time domain allocation within a slot: start symbol index (0...12).
    unsigned start_symbol_index;
    /// Time domain allocation within a slot: number of symbols (1...14).
    unsigned nof_symbols;
    /// DMRS symbol positions as a mask.
    std::array<bool, MAX_NSYMB_PER_SLOT> dmrs_symb_pos;
    /// Indicates the DMRS configuration type.
    dmrs_type dmrs_config_type;
    /// Number of DMRS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Provides \f$n_{ID}\f$ from TS38.211 section 6.3.1.1 Scrambling. Range is (0...1023).
    unsigned n_id;
    /// Scaling factor to apply to the resource elements according to PUSCH power allocation in TS38.214.
    float scaling;
    /// Reserved RE pattern where PUSCH is not mapped.
    re_pattern_list reserved;
    /// Port indexes the PUSCH transmission is mapped onto.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~pusch_demodulator() = default;

  /// \brief Demodulates a PUSCH transmission described in TS38.211 Section 7.3.1.
  ///
  /// \remarks
  /// - The size of \c data determines the codeword size in LLR.
  /// - The size of \c harq_ack determines the number of LLR for HARQ-ACK.
  /// - The size of \c csi_part1 determines the number of LLR for CSI Part1.
  /// - The size of \c csi_part2 determines the number of LLR for CSI Part2.
  /// - The total number of LLR must be consistent with the number of RE allocated in the transmission.
  ///
  /// \param[out] data      Codeword LLR destination buffer.
  /// \param[out] harq_ack  HARQ-ACK LLR destination buffer.
  /// \param[out] csi_part1 CSI Part1 LLR destination buffer.
  /// \param[out] csi_part2 CSI Part2 LLR destination buffer.
  /// \param[in] grid       Provides the source resource grid.
  /// \param[in] estimates  Provides the channel estimates.
  /// \param[in] config     Provides the configuration parameters.
  virtual void demodulate(data_llr_buffer&            data,
                          harq_ack_llr_buffer&        harq_ack,
                          csi_part1_llr&              csi_part1,
                          csi_part2_llr&              csi_part2,
                          const resource_grid_reader& grid,
                          const channel_estimate&     estimates,
                          const config_t&             config) = 0;
};

} // namespace srsgnb
