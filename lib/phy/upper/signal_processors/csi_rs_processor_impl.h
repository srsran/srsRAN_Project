/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_IMPL_H

#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"

namespace srsgnb {

/// Defines a NZP-CSI-RS signal generator.
class csi_rs_processor_impl : public csi_rs_processor
{
private:
  /// Maximum number of PRB subcarriers that the CSI-RS can occupy, deduced from TS 38.211 Table 7.4.1.5.3-1.
  static constexpr unsigned MAX_SUBCS_PRB = 4;
  /// Maximum currently supported ports for the CSI-RS generation.
  static constexpr unsigned CSI_RS_MAX_PORTS = 16;
  /// Maximum sequence length for one OFDM symbol.
  static constexpr unsigned MAX_SEQ_LEN = MAX_SUBCS_PRB * MAX_RB;

  /// \brief CDM sequence element struct.
  /// \remark This struct is used to build the CDM sequence tables.
  struct cdm_sequence {
    /// Frequency code sequences \f$w_f\f$.
    static_vector<float, 2> w_f;
    /// Time code sequences \f$w_t\f$.
    static_vector<float, 4> w_t;
  };

  /// FD-CDM2 sequence table, as defined in TS 38.211 Table 7.4.1.5.3-3.
  static const std::array<const cdm_sequence, 2> fd_cdm2_table;
  /// CDM4-FD2-TD2 sequence table, as defined in TS 38.211 Table 7.4.1.5.3-4.
  static const std::array<const cdm_sequence, 4> cdm4_fd2_td2_table;
  /// CDM8-FD2-TD4 sequence table, as defined in TS 38.211 Table 7.4.1.5.3-5.
  static const std::array<const cdm_sequence, 8> cdm8_fd2_td4_table;

  /// Pseudo-random sequece generator for the NZP-CSI-RS signal.
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  /// \brief Generates the NZP-CSI-RS sequence.
  ///
  /// This method implements the NZP-CSI-RS sequence generation, as described in TS 38.211 Section 7.4.1.5.2.
  /// The generated sequence does not include the CDM processing.
  ///
  /// \param[out] sequence The returned NZP-CSI-RS sequence.
  /// \param[in] symbol The symbol index within the slot.
  /// \param[in] config Provides the higher layer parameters necessary to generate the sequence.
  ///
  /// \note The generated sequence length is determined by the size of the provided \c sequence.
  void generate_sequence(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Generates the resource grid allocation patterns for the CSI-RS signals.
  ///
  /// This method implements the CSI-RS signal mapping as described in TS 38.211 Section 7.4.1.5.3.
  ///
  /// \param[out] pattern_list Provides a list of RE patterns that contain the CSI-RS mapping for each port.
  /// \param[in] config Provides the higher layer parameters that determine the resource grid mapping.
  ///
  /// \note This method expects \c pattern_list to contain as many RE patterns as the number of ports in the config.
  void build_grid_allocation(span<re_pattern> pattern_list, const config_t& config);

  /// \brief Applies the CDM codes to the NZP-CSI-RS sequence.
  ///
  /// This helper method takes an NZP-CSI-RS sequence pertaining to a single OFDM symbol, and applies the corresponding
  /// CDM code, as defined in TS 38.211 Section 7.4.1.5.3.
  ///
  /// \param[out] seq_out The returned NZP-CSI-RS sequence with CDM.
  /// \param[in] seq_in The input sequence.
  /// \param[in] cdm The CDM type.
  /// \param[in] cdm_idx The applied CDM sequence.
  /// \param[in] l_idx The \f$l'\f$ symbol index within the \f$w_t\f$ TD-CDM sequence.
  void apply_cdm(span<cf_t>            seq_out,
                 span<const cf_t>      seq_in,
                 const csi_rs_cdm_type cdm,
                 const unsigned        cdm_idx,
                 const unsigned        l_idx);

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_IMPL_H
