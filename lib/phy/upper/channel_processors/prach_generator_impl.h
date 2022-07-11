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

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/upper/channel_processors/prach_generator.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// \brief On-demand PRACH time-domain signal generator.
///
/// It generates PRACH time-domain signals on demand instead of generating and storing. It minimizes memory footprint at
/// the cost of longer processing time.
class prach_generator_impl : public prach_generator
{
private:
  /// Indicates that a value is reserved.
  static constexpr unsigned RESERVED = UINT32_MAX;
  /// Maximum PRACH length in subframes.
  static constexpr unsigned MAX_PRACH_LENGTH_SF = 4;

  /// Uplink grid size in PRB.
  unsigned nof_prb_ul_grid;
  /// DFT size for 15kHz subcarrier spacing.
  unsigned dft_size_15kHz;
  /// DFT for long frequency-domain sequence generation.
  std::unique_ptr<dft_processor> dft_l839;
  /// DFT for short frequency-domain sequence generation.
  std::unique_ptr<dft_processor> dft_l139;
  /// DFT for preambles 0, 1 and 2.
  std::unique_ptr<dft_processor> dft_1_25_kHz;
  /// DFT for preamble 3.
  std::unique_ptr<dft_processor> dft_5_kHz;
  /// DFT for short preambles (A1, A2, A3, B1, B2, B3, B4, C0 and C2).
  std::unique_ptr<dft_processor> dft_short;
  /// Temporal sequence storage.
  srsvec::aligned_vec<cf_t> temp;

  /// \brief Calculates the number of cyclic shifts \f$N_{CS}\f$ used in the sequence generation as per TS38.211
  /// Section 6.3.3.1.
  ///
  /// The number of cyclic shifts \f$N_{CS}\f$ extracted from:
  /// - TS38.211 Table 6.3.3.1-5 for preambles 0, 1 and 2
  /// - TS38.211 Table 6.3.3.1-6 for preamble 3, and
  /// - TS38.211 Table 6.3.3.1-7 for the remaining preambles.
  ///
  /// \param[in] prach_scs             \brief PRACH subcarrier spacing (parameter \f$\Delta f^{RA}\f$ as defined in
  ///                                  TS38.211 Section 6.3.3.1).
  /// \param[in] restricted_set        \brief Restricted set (parameter \c restrictedSetConfig defined in TS38.331
  ///                                  RACH-ConfigCommon).
  /// \param[in] zero_correlation_zone \brief Zero correlation zone (parameter \c zeroCorrelationZoneConfig defined in
  ///                                  TS38.331 RACH-ConfigGeneric).
  /// \return The number of cyclic shifts if the input parameters are valid. Otherwise, RESERVED.
  static unsigned
  get_nof_cyclic_shifts(unsigned prach_scs, restricted_set_config restricted_set, unsigned zero_correlation_zone);

  /// Determines the sequence length \f$L_{RA}\f$ as per TS38.211 Table 6.3.3.1-1.
  static unsigned get_sequence_length(preamble_format format);

  /// Calculates sequence number \f$u\f$ as per TS38.211 Table 6.3.3.1-3.
  static unsigned get_sequence_number_long(unsigned root_sequence_index);

  /// \brief Gets the parameter \f$N_{RB}^{RA}\f$ as per TS38.211 Table 6.3.3.2-1.
  /// \param[in] prach_scs_Hz Parameter \f$\Delta f^{RA}\f$ for PRACH.
  /// \param[in] pusch_scs_Hz Parameter \f$\Delta f\f$ for PUSCH.
  /// \return Return a valid \f$N_{RB}^{RA}\f$ if the preamble is implemented. Otherwise, \c RESERVED.
  static unsigned get_N_rb_ra(unsigned prach_scs_Hz, unsigned pusch_scs_Hz);

  /// \brief Gets the parameter \f$\bar{k}\f$ as per TS38.211 Table 6.3.3.2-1.
  /// \param[in] prach_scs_Hz Parameter \f$\Delta f^{RA}\f$ for PRACH.
  /// \param[in] pusch_scs_Hz Parameter \f$\Delta f\f$ for PUSCH.
  /// \return Return a valid \f$\bar{k}\f$ if the preamble is implemented. Otherwise, \c RESERVED.
  static unsigned get_k_bar(unsigned prach_scs_Hz, unsigned pusch_scs_Hz);

  /// \brief Generates the \f$y_{u,v}\f$ sequence.
  /// \param[in] u   Sequence number.
  /// \param[in] C_v Sequence shift.
  /// \return View of the generated sequence.
  span<const cf_t> generate_y_u_v_long(unsigned u, unsigned C_v);

  /// \brief Modulates PRACH as per TS38.211 Section 5.3.2.
  /// \param[in] y_u_v  Frequency-domain signal to modulate.
  /// \param[in] config PRACH configuration.
  /// \return View of the generated sequence.
  span<const cf_t> modulate(span<const cf_t> y_u_v, const configuration& config);

public:
  /// \brief PRACH generator constructor.
  ///
  /// The PRACH generator depends on the DFT to generate the time-domain signals.
  ///
  /// \param nof_prb_ul_grid_  Number of PRB for the UL resource grid.
  /// \param dft_size_15kHz_   DFT size for 15kHz subcarrier spacing.
  /// \param dft_l839_         DFT processor for generating long sequences.
  /// \param dft_l139_         DFT processor for generating short sequences.
  /// \param dft_1_25_kHz_ DFT processor for subcarrier spacing of 1.25kHz.
  /// \param dft_5_kHz_        DFT processor for subcarrier spacing of 5kHz.
  /// \param dft_short_        DFT processor for short preambles.
  prach_generator_impl(unsigned                       nof_prb_ul_grid_,
                       unsigned                       dft_size_15kHz_,
                       std::unique_ptr<dft_processor> dft_l839_,
                       std::unique_ptr<dft_processor> dft_l139_,
                       std::unique_ptr<dft_processor> dft_1_25_kHz_,
                       std::unique_ptr<dft_processor> dft_5_kHz_,
                       std::unique_ptr<dft_processor> dft_short_) :
    nof_prb_ul_grid(nof_prb_ul_grid_),
    dft_size_15kHz(dft_size_15kHz_),
    dft_l839(std::move(dft_l839_)),
    dft_l139(std::move(dft_l139_)),
    dft_1_25_kHz(std::move(dft_1_25_kHz_)),
    dft_5_kHz(std::move(dft_5_kHz_)),
    dft_short(std::move(dft_short_)),
    temp(MAX_PRACH_LENGTH_SF * 15 * dft_size_15kHz)
  {
    srsran_assert(dft_l839, "Invalid L839 DFT pointer");
    srsran_assert(dft_l839->get_direction() == dft_processor::direction::DIRECT, "Invalid L839 DFT direction");
    srsran_assert(dft_l839->get_size() == 839U, "Invalid L839 DFT size");

    srsran_assert(dft_l139, "Invalid L139 DFT pointer");
    srsran_assert(dft_l139->get_direction() == dft_processor::direction::DIRECT, "Invalid L139 DFT direction");
    srsran_assert(dft_l139->get_size() == 139U, "Invalid L139 DFT size");

    srsran_assert(dft_1_25_kHz, "Invalid 1k25 DFT pointer");
    srsran_assert(dft_1_25_kHz->get_direction() == dft_processor::direction::INVERSE, "Invalid 1k25 DFT direction");
    srsran_assert(dft_1_25_kHz->get_size() == (dft_size_15kHz * 15000) / 1250, "Invalid 1k25 DFT size");

    srsran_assert(dft_5_kHz, "Invalid 5k DFT pointer");
    srsran_assert(dft_5_kHz->get_direction() == dft_processor::direction::INVERSE, "Invalid 5k DFT direction");
    srsran_assert(dft_5_kHz->get_size() == (dft_size_15kHz * 15000) / 5000, "Invalid 5k DFT size");
  }

  // See interface for documentation.
  span<const cf_t> generate(const configuration& config) override;
};

} // namespace srsgnb
