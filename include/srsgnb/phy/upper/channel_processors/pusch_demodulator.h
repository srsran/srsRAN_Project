/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_MODULATOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_MODULATOR_H

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"

namespace srsgnb {

/// \brief Describes a PUSCH demodulator interface.
///
/// The PUSCH demodulator must:
/// - extract the RE for a PUSCH transmission,
/// - equalize the extracted symbols,
/// - perform soft demodulation,
/// - undo scrambling sequences, and
/// - undo data and control multiplexing.
class pusch_demodulator
{
public:
  /// Maximum number of RE per codeword in a single transmission.
  static constexpr unsigned MAX_CODEWORD_SIZE = MAX_RB * NRE * MAX_NSYMB_PER_SLOT * MAX_PORTS / 2;

  /// Maximum number of LLR per codeword in a single transmission.
  static constexpr unsigned MAX_NOF_DATA_LLR = MAX_CODEWORD_SIZE * 8;

  /// Maximum number of LLR for HARQ-ACK in a single transmission.
  static constexpr unsigned MAX_NOF_HARQ_ACK_LLR = 1024;

  /// Maximum number of LLR for CSI Part1 in a single transmission.
  static constexpr unsigned MAX_NOF_CSI_PART1_LLR = 1024;

  /// Maximum number of LLR for CSI Part2 in a single transmission.
  static constexpr unsigned MAX_NOF_CSI_PART2_LLR = 1024;

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

  /// \brief Modulates a PUSCH codeword according to TS38.211 section 7.3.1 Physical downlink shared channel.
  ///
  /// \param[out] grid Provides the source resource grid.
  /// \param[in] grid Provides the source resource grid.
  /// \param[in] codewords Provides the encoded codewords to modulate.
  /// \param[in] config Provides the configuration reference.
  /// \note The number of codewords shall be consistent with the number of layers.
  /// \note The codeword length shall be consistent with the resource mapping, considering the reserved resource
  /// elements.
  virtual void modulate(data_llr_buffer&            data,
                        harq_ack_llr_buffer&        harq_ack,
                        csi_part1_llr&              csi_part1,
                        csi_part2_llr&              csi_part2,
                        const resource_grid_reader& grid,
                        const config_t&             config) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_MODULATOR_H
