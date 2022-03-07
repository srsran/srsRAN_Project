/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SIGNAL_PROCESSORS_DMRS_PDSCH_PROCESSOR_IMPL_H_
#define SRSGNB_SIGNAL_PROCESSORS_DMRS_PDSCH_PROCESSOR_IMPL_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsgnb/support/math_utils.h"

namespace srsgnb {

/// Defines a DMRS for PDSCH generic implementation.
class dmrs_pdsch_processor_impl : public dmrs_pdsch_processor
{
private:
  /// Defines the maximum number of DMRS per RB.
  static constexpr unsigned MAX_DRMS_PER_RB =
      std::max(dmrs_nof_dmrs_per_rb(dmrs_type::TYPE1), dmrs_nof_dmrs_per_rb(dmrs_type::TYPE2));

  /// Define the maximum number of DMRS per symbol.
  static constexpr unsigned MAX_DMRS_PER_SYMBOL = MAX_RB * MAX_DRMS_PER_RB;

  /// Defines generation parameters.
  struct params_t {
    unsigned             delta;
    std::array<float, 2> w_f;
    std::array<float, 2> w_t;
  };

  /// Provides TS 38.211 Table 7.4.1.1.2-1: Parameters for PDSCH DM-RS configuration type 1.
  static const std::array<params_t, 8> params_type1;

  /// Provides TS 38.211 Table 7.4.1.1.2-2: Parameters for PDSCH DM-RS configuration type 2.
  static const std::array<params_t, 12> params_type2;

  /// Pseudo-random sequence generator instance.
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  /// \brief Helper function to determine the DMRS for PDSCH amplitude.
  ///
  /// This method determines the DMRS for PDSCH amplitude from the number of DMRS CDM groups without data. It is
  /// implemented according to TS 38.214 V15.10.0 tables 4.1-1 and 6.2.2-1 for PDSCH and PUSCH respectively.
  ///
  /// \param[in] nof_dmrs_cdm_groups_without_data Provides the number of DMRS CDM groups without data.
  /// \return The amplitude of the DMRS for PDSCH.
  static inline unsigned get_amplitude(unsigned nof_dmrs_cdm_groups_without_data)
  {
    static constexpr std::array<float, 3> ratio_dB = {0, -3, -4.77};

    srsran_assert(nof_dmrs_cdm_groups_without_data > 0 && nof_dmrs_cdm_groups_without_data < 3,
                  "Invalid number of DMRS CDM groups without data (%d)",
                  nof_dmrs_cdm_groups_without_data);

    return convert_dB_to_amplitude(-ratio_dB[nof_dmrs_cdm_groups_without_data - 1]);
  }

  /// \brief Implements TS 38.211 section 7.4.1.1.1 Sequence generation.
  ///
  /// This method generates the sequence described in TS 38.211 section 7.4.1.1.1, considering the only values required
  /// in TS 38.211 section 7.4.1.1.2.
  ///
  /// \param[out] sequence Provides the sequence destination.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required parameters to calculate the sequences.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const config_t& config) const;

  /// \brief Implements TS 38.211 section 7.4.1.1.2 Mapping to physical resources.
  ///
  /// This method implements the signal mapping as described in TS 38.211 section 7.4.1.1.2. In addition, it applies
  /// precoding if configured.
  ///
  /// \param[out] grid Provides the grid destination to map the signal.
  /// \param[in] sequence Provides the generated sequence for the given symbol.
  /// \param[in] base_mask Provides a base subcarrier allocation mask for the given symbol.
  /// \param[in] symbol Denotes the symbol index.
  /// \param[in] config Provides the required fields to map the signal.
  /// \note The method expects \c base_mask to have \c NRE entries padding as \c false at the beginning.
  void mapping(resource_grid_writer& grid,
               span<const cf_t>      sequence,
               span<const bool>      base_mask,
               unsigned              symbol,
               const config_t&       config);

  /// Temporal resource element storage, just in case weights or precoding is applied for each port.
  static_vector<static_vector<cf_t, MAX_DMRS_PER_SYMBOL>, MAX_PORTS> temp_re;

public:
  void map(resource_grid_writer& grid, const config_t& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_SIGNAL_PROCESSORS_DMRS_PDSCH_PROCESSOR_IMPL_H_
