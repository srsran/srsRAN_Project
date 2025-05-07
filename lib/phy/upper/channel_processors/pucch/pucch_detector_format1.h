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
/// PUCCH Format 1 detector declaration.

#pragma once

#include "srsran/adt/tensor.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_format1_map.h"
#include "srsran/phy/upper/equalization/channel_equalizer.h"
#include "srsran/phy/upper/equalization/dynamic_ch_est_list.h"
#include "srsran/phy/upper/pucch_helper.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/cyclic_prefix.h"
#include <memory>

namespace srsran {

/// PUCCH Format 1 detector.
class pucch_detector_format1
{
public:
  /// \brief Number of available initial cyclic shifts.
  static constexpr unsigned nof_initial_cyclic_shifts = pucch_constants::format1_initial_cyclic_shift_range.stop();
  /// \brief Maximum number of available time-domain OCC indices.
  static constexpr unsigned nof_time_domain_occs = pucch_constants::format1_time_domain_occ_range.stop();
  /// \brief Maximum number of REs allocated to PUCCH Format 1.
  ///
  /// The allocated resources are at most one PRB over all OFDM symbols.
  static constexpr unsigned MAX_ALLOCATED_RE_F1 = NRE * MAX_NSYMB_PER_SLOT;

  /// \brief Constructor: provides access to a collection of low-PAPR sequences and a pseudorandom sequence generator.
  /// \param[in] low_papr_         Collection of low-PAPR sequences.
  /// \param[in] pseudo_random_    Pseudo-random sequence generator.
  /// \param[in] dft_              12-point discrete Fourier transform processor.
  /// \param[in] idft_             12-point inverse discrete Fourier transform processor.
  /// \remark The low-PAPR collection should correspond to the cyclic shifts \f$\{\alpha : \alpha = 2 \pi
  /// \frac{n}{N_{\textup{sc}}^{\textup{RB}}}, \quad n = 0, \dots, N_{\textup{sc}}^{\textup{RB}}-1\}\f$, where
  /// \f$N_{\textup{sc}}^{\textup{RB}} = 12\f$ is the number of subcarriers in a resource block.
  pucch_detector_format1(std::unique_ptr<low_papr_sequence_collection> low_papr_,
                         std::unique_ptr<pseudo_random_generator>      pseudo_random_,
                         std::unique_ptr<dft_processor>                dft_,
                         std::unique_ptr<dft_processor>                idft_) :
    low_papr(std::move(low_papr_)),
    helper(std::move(pseudo_random_)),
    dft(std::move(dft_)),
    idft(std::move(idft_)),
    dmrs_mask(MAX_NSYMB_PER_SLOT)
  {
    srsran_assert(low_papr, "Invalid Low PAPR sequence generator.");
    srsran_assert(dft, "Invalid DFT processor");
    srsran_assert(dft->get_direction() == dft_processor::direction::DIRECT, "DFT processor direction set to INVERSE");
    srsran_assert(dft->get_size() == NRE, "DFT processor size is {}, should be {}.", dft->get_size(), NRE);
    srsran_assert(idft, "Invalid inverse DFT processor");
    srsran_assert(idft->get_direction() == dft_processor::direction::INVERSE,
                  "Inverse DFT processor direction set to DIRECT");
    srsran_assert(idft->get_size() == NRE, "Inverse DFT processor size is {}, should be {}.", dft->get_size(), NRE);
  }

  /// Detects multiplexed PUCCH Format 1 transmissions. See \ref pucch_detector for more details.
  const pucch_format1_map<pucch_detector::pucch_detection_result_csi>&
  detect(const resource_grid_reader&                  grid,
         const pucch_detector::format1_configuration& config,
         const pucch_format1_map<unsigned>&           mux_nof_harq_ack);

private:
  /// Hop contributions for EPRE and noise variance computations, common to all multiplexed PUCCH transmissions.
  struct hop_contribution_common {
    /// Contribution to the estimated EPRE.
    float epre_contribution;
    /// Number of samples the EPRE contribution is based on.
    unsigned nof_epre_samples;
    /// Contribution to the estimated noise variance.
    float noise_var_contribution;
    /// Number of samples the noise variance contribution is based on.
    unsigned nof_noise_samples;
  };

  /// Hop contributions for metric and RSRP computations, specific for each multiplexed PUCCH transmsission.
  struct hop_contribution_mux {
    /// Contribution to the detection metric obtained from the power of signal and DM-RS.
    float main_contribution;
    /// Contribution to the detection metric obtained from the cross product between signal and DM-RS.
    cf_t cross_contribution;
    /// Constribution to the estimated RSRP.
    float rsrp_contribution;
  };

  /// \brief Processes the contents of one hop and computes the contributions to the detection metrics.
  ///
  /// Extracts the PUCCH resources from the resource grid and computes, for each multiplexed PUCCH, the hop
  /// contributions to the detection metric.
  /// \param[out] hop_metrics       Map of contributions to the detection metric for each multiplexed PUCCH (identified
  ///                               by initial cyclic shift and time-domain orthogonal cover code).
  /// \param[in]  grid              Resource grid with the received signal samples for the current slot.
  /// \param[in]  config            Common configuration for all multiplexed PUCCH transmissions (fields \c
  ///                               time_domain_occ, \c initial_cyclic_shift and \c nof_harq_ack are neglected).
  /// \param[in]  mux_nof_harq_ack  Map of ACK lengths (i.e., parameter \c nof_harq_ack) for all multiplexed PUCCH
  ///                               transmissions, identified by initial cyclic shift and time-domain orthogonal cover
  ///                               code.
  /// \param[in]  i_hop             Hop index (either 0 or 1).
  /// \return The estimated EPRE and noise variance (without normalizing by the number of samples) and the number of
  /// samples used for the estimations.
  hop_contribution_common
  process_hop(static_vector<hop_contribution_mux, nof_initial_cyclic_shifts * nof_time_domain_occs>& hop_metrics,
              const resource_grid_reader&                                                            grid,
              const pucch_detector::format1_configuration&                                           config,
              const pucch_format1_map<unsigned>&                                                     mux_nof_harq_ack,
              unsigned                                                                               i_hop);

  /// \brief Reconstructs the noiseless received DM-RS signal.
  ///
  /// \param[out] dmrs_reconstructed  Reconstructed DM-RS signals for all ports and symbols (superimposing contributions
  ///                                 from all PUCCH transmissions).
  /// \param[in]  ch                  Estimated channels for all ICS and the current OCC (see \c w_star_dmrs).
  /// \param[in]  w_star_dmrs         Current time-domain orthogonal cover code.
  /// \param[in]  is_first            Boolean flag denoting that the current contribution is the first one and should
  ///                                 overwrite (not combine with) the content of \c dmrs_reconstructed.
  void combine_reconstructed_contributions(
      static_tensor<3, cf_t, MAX_PORTS * pucch_detector_format1::MAX_ALLOCATED_RE_F1 / 2>& dmrs_reconstructed,
      const static_tensor<2, cf_t, NRE * MAX_PORTS>&                                       ch,
      span<const cf_t>                                                                     w_star_dmrs,
      bool                                                                                 is_first);

  /// Collection of low-PAPR sequences.
  std::unique_ptr<low_papr_sequence_collection> low_papr;
  /// Pseudorandom sequence generator.
  pucch_helper helper;
  /// \brief DFT processor.
  /// \remark The size of the DFT must be 12 (same as the number of REs per PRB and the number of possible initial
  /// cyclic shifts).
  std::unique_ptr<dft_processor> dft;
  /// \brief Inverse DFT processor.
  /// \remark The size of the inverse DFT must be 12 (same as the number of REs per PRB and the number of possible
  /// initial cyclic shifts).
  std::unique_ptr<dft_processor> idft;
  /// Boolean mask identifying OFDM symbols carrying PUCCH DM-RS (marked as \c true).
  bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_mask;
  /// Storage of detection results for multiplexed PUCCH transmissions.
  pucch_format1_map<pucch_detector::pucch_detection_result_csi> mux_results;
};

} // namespace srsran
