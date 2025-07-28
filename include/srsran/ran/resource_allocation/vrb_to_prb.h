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

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_block.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>

namespace srsran::vrb_to_prb {

enum class mapping_type : uint8_t { non_interleaved = 0, interleaved_n2 = 2, interleaved_n4 = 4 };

/// PDSCH VRB-to-PRB mapping configuration.
struct configuration {
  /// Indicates the number of RB bundles \f$N_{bundle}\f$. It is set to zero for non-interleaved mapping.
  unsigned nof_bundles = 0;
  /// Indicates PRB index of the lowest PRB in the CORESET used for the DCI transmission.
  unsigned coreset_start = 0;
  /// Indicates the size of the set of VRBs defined in the mapping. Only applicable to interleaved mapping.
  unsigned nof_rbs = 0;
  /// Indicates the first RB bundle size. Only applicable to interleaved mapping.
  unsigned first_bundle_size = 0;
  /// Indicates the other RB bundle sizes. Only applicable to interleaved mapping. Possible values: {2, 4}.
  unsigned other_bundle_size = 0;
  /// Indicates the last RB bundle size. Only applicable to interleaved mapping.
  unsigned last_bundle_size = 0;

  /// \brief Checks whether the resource allocation is interleaved.
  ///
  /// The resource allocation is interleaved if the number of bundles is not zero.
  ///
  /// \return True if the resource allocation is interleaved. Otherwise, False.
  bool is_interleaved() const { return nof_bundles != 0; }

  /// Equal comparison between two VRB-to-PRB configurations.
  bool operator==(const configuration& other) const
  {
    return (nof_bundles == other.nof_bundles) && (coreset_start == other.coreset_start) && (nof_rbs == other.nof_rbs) &&
           (first_bundle_size == other.first_bundle_size) && (other_bundle_size == other.other_bundle_size) &&
           (last_bundle_size == other.last_bundle_size);
  }
};

/// \brief Creates a non-interleaved VRB-to-PRB configuration object for PDSCH transmissions scheduled by DCI Format 1_0
/// in common SS.
///
/// Implemented as per TS38.211 Section 7.3.1.6 Case 1:
/// - A virtual resource \f$n\f$ is mapped to the physical block \f$n + N_{start}^{CORESET}\f$.
///
/// If the PDSCH is scheduled with DCI format 1_0 with the CRC scrambled by SI-RNTI in Type0-PDCCH common search space
/// in CORESET 0, the BWP start and size must match with the CORESET0 start and size. Consequently,
/// \f$N_{start}^{CORESET}\f$ must be zero.
///
/// \param[in] N_start_coreset PRB index of the lowest-numbered RB in the CORESET used for the DCI transmission as
/// \f$N_{start}^{CORESET}\f$.
/// \return A non-interleaved VRB-to-PRB configuration object.
inline configuration create_non_interleaved_common_ss(unsigned N_start_coreset)
{
  return {.coreset_start = N_start_coreset};
}

/// \brief Creates a non-interleaved VRB-to-PRB configuration object for PDSCH transmissions scheduled by DCI other than
/// Format 1_0 in common SS.
///
/// Implemented as per TS38.211 Section 7.3.1.6 Case 2:
/// - A virtual resouce \f$n\f$ is mapped to the physical block \f$n\f$.
///
/// \return A non-interleaved VRB-to-PRB configuration object.
inline configuration create_non_interleaved_other()
{
  return {};
}

/// \brief Creates an interleaved VRB-to-PRB configuration for PDSCH transmissions scheduled with DCI Format 1_0 with
/// the CRC scrambled by SI-RNTI in Type0-PDCCH common SS in CORESET0.
///
/// Implemented as per TS38.211 Section 7.3.1.6 Case 3:
/// - the number of bundles is calculated as \f$N_{bundle}=\lceil N_{BWP,init}^{size}/L \rceil\f$,
/// - the last RB bundle consists of \f$N_{BWP,init}^{size} \bmod L\f$ if \f$N_{BWP,init}^{size} \bmod L > 0\f$,
/// - all other resource block bundles consist of \f$L\f$ RBs, and
/// - the bundle size is \f$L=2\f$.
///
///
/// \param[in] N_start_coreset PRB index of the lowest-numbered RB in the CORESET used for the DCI transmission as
/// \f$N_{start}^{CORESET}\f$.
/// \param[in] N_bwp_init_size The size in RBs of CORESET0 as \f$N_{BWP,init}^{size}\f$.
/// \return An interleaved VRB-to-PRB configuration object.
inline configuration create_interleaved_coreset0(unsigned N_start_coreset, unsigned N_bwp_init_size)
{
  // CORESET0 constant bundle size.
  static constexpr unsigned L = 2;

  const unsigned nof_bundles       = divide_ceil(N_bwp_init_size, L);
  const unsigned coreset_start     = N_start_coreset;
  const unsigned nof_rbs           = N_bwp_init_size;
  const unsigned first_bundle_size = L;
  const unsigned other_bundle_size = L;
  const unsigned last_bundle_size  = (N_bwp_init_size % L) > 0 ? N_bwp_init_size % L : L;

  return {nof_bundles, coreset_start, nof_rbs, first_bundle_size, other_bundle_size, last_bundle_size};
}

/// \brief Creates an interleaved VRB-to-PRB configuration for PDSCH transmissions scheduled by DCI format 1_0 in any
/// common SS except Type0-PDCCH common SS in CORESET0.
///
/// Implemented as per TS38.211 Section 7.3.1.6 Case 4:
/// - the set of \f$N_{BWP,init}^{size}\f$ virtual RBs starting at \f$N_{BWP,i}^{start}\f$ is divided into
/// \f$N_{bundle}=\lceil (N_{BWP,init}^{size} + ((N_{BWP,i}^{start} + N_{start}^{CORESET}) \bmod L))/L
/// \rceil\f$,
/// - the bundle size is \f$L=2\f$,
/// - the first RB bundle consists of \f$L - ((N_{BWP,init}^{size} + N_{start}^{CORESET}) \bmod L)\f$,
/// - the last RB bundle consists of \f$(N_{BWP,init}^{size} + N_{BWP,i}^{start} + N_{start}^{CORESET}) \bmod L\f$
/// if \f$(N_{BWP,init}^{size} + N_{BWP,i}^{start} + N_{start}^{CORESET}) \bmod L > 0\f$ and \f$L\f$ RB otherwise, and
/// - all other resource block bundles consist of \f$L\f$ RBs.
///
/// \param[in] N_start_coreset PRB index of the lowest-numbered RB in the CORESET used for the DCI transmission as
/// \f$N_{start}^{CORESET}\f$.
/// \param[in] N_bwp_i_start BWP \f$i\f$ starting position \f$N_{BWP,i}^{start}\f$.
/// \param[in] N_bwp_init_size The size in RBs of CORESET0 if available, otherwise the initial bandwidth part size as
/// \f$N_{BWP,init}^{size}\f$.
/// \return An interleaved VRB-to-PRB configuration object.
inline configuration
create_interleaved_common_ss(unsigned N_start_coreset, unsigned N_bwp_i_start, unsigned N_bwp_init_size)
{
  // Constant bundle size.
  static constexpr unsigned L = 2;

  const unsigned nof_bundles       = divide_ceil(N_bwp_init_size + ((N_bwp_i_start + N_start_coreset) % L), L);
  const unsigned coreset_start     = N_start_coreset;
  const unsigned nof_rbs           = N_bwp_init_size;
  const unsigned first_bundle_size = L - ((N_bwp_i_start + N_start_coreset) % L);
  const unsigned other_bundle_size = L;
  const unsigned last_bundle_size  = ((N_bwp_init_size + N_bwp_i_start + N_start_coreset) % L) > 0
                                         ? (N_bwp_init_size + N_bwp_i_start + N_start_coreset) % L
                                         : L;

  return {nof_bundles, coreset_start, nof_rbs, first_bundle_size, other_bundle_size, last_bundle_size};
}

/// \brief Creates an interleaved VRB-to-PRB configuration for all other PDSCH transmissions with interleaved mapping.
///
/// Implemented as per TS38.211 Section 7.3.1.6 Case 5:
/// - the set of \f$N_{BWP,i}^{size}\f$ virtual RBs starting at \f$N_{BWP,i}^{start}\f$ is divided into
/// \f$N_{bundle}=\lceil (N_{BWP,i}^{size} + (N_{BWP,i}^{start} \bmod L_i))/L_i \rceil\f$,
/// - the first RB bundle consists of \f$L_i - N_{BWP,i}^{size} \bmod L_i\f$,
/// - the last RB bundle consists of \f$(N_{BWP,i}^{size}+N_{BWP,i}^{start}) \bmod L_i\f$ if
/// \f$(N_{BWP,i}^{size}+N_{BWP,i}^{start}) \bmod L_i > 0\f$ and \f$L_i\f$ RBs otherwise,
/// - all other resource block bundles consist of \f$L_i\f$ RBs.
///
/// \param[in] N_bwp_i_start BWP \f$i\f$ starting position \f$N_{BWP,i}^{start}\f$.
/// \param[in] N_bwp_i_size  BWP \f$i\f$ size\f$N_{BWP,i}^{size}\f$.
/// \param[in] L_i           RB Bundle size for BWP \f$i\f$ in RBs \f$L_i\f$.
/// \return An interleaved VRB-to-PRB configuration object.
inline configuration create_interleaved_other(unsigned N_bwp_i_start, unsigned N_bwp_i_size, mapping_type L_i)
{
  unsigned L = static_cast<unsigned>(L_i);
  srsran_assert(L == 2 || L == 4, "Invalid bundle size {}. Valid values are 2 or 4.", L);
  const unsigned nof_bundles       = divide_ceil(N_bwp_i_size + (N_bwp_i_start % L), L);
  const unsigned coreset_start     = 0;
  const unsigned nof_rbs           = N_bwp_i_size;
  const unsigned first_bundle_size = L - (N_bwp_i_start % L);
  const unsigned other_bundle_size = L;
  const unsigned last_bundle_size  = ((N_bwp_i_start + N_bwp_i_size) % L) > 0 ? (+N_bwp_i_start + N_bwp_i_size) % L : L;

  return {nof_bundles, coreset_start, nof_rbs, first_bundle_size, other_bundle_size, last_bundle_size};
}

/// \brief Describes a non-interleaved VRB-to-PRB mapping.
class non_interleaved_mapping
{
private:
  configuration config;

public:
  non_interleaved_mapping(const configuration& config_) : config(config_)
  {
    srsran_assert(not config.is_interleaved(), "Invalid configuration for non-interleaved mapping.");
  }

  /// \brief Converts a VRB to its corresponding PRB.
  ///
  /// \param[in] vrb VRB to be converted.
  /// \return A PRB.
  unsigned vrb_to_prb(unsigned vrb) const;

  /// \brief Converts a PRB to its corresponding VRB.
  ///
  /// \param[in] prb PRB to be converted.
  /// \return A VRB.
  unsigned prb_to_vrb(unsigned prb) const;

  /// \brief Converts a VRB interval to its corresponding PRB interval.
  ///
  /// \param[in] vrbs VRB interval to be converted.
  /// \return A PRB interval.
  prb_interval vrb_to_prb(const vrb_interval& vrbs) const;

  /// \brief Converts a PRB interval to its corresponding VRB interval.
  ///
  /// \param[in] prbs PRB interval to be converted.
  /// \return A VRB interval.
  vrb_interval prb_to_vrb(const prb_interval& prbs) const;

  /// \brief Converts a VRB bitmap to a PRB bitmap.
  ///
  /// \param[in] bwp_size BWP size in CRB as \f$N_{BWP,i}^{size}\f$.
  /// \param[in] vrbs     VRB bitmap to be converted.
  /// \return A PRB bitmap.
  prb_bitmap vrb_to_prb(unsigned bwp_size, const vrb_bitmap& vrbs) const;

  /// \brief Converts a PRB bitmap to a VRB bitmap.
  ///
  /// \param[in] prbs PRB bitmap to be converted.
  /// \return A VRB bitmap.
  vrb_bitmap prb_to_vrb(const prb_bitmap& prbs) const;

  /// \brief Converts a VRB bitmap to a CRB bitmap.
  ///
  /// \param[in] bwp_start   Lowest CRB index of the BWP relative to CRB0 (PointA) as \f$N_{BWP,i}^{start}\f$.
  /// \param[in] bwp_size    BWP size in CRB as \f$N_{BWP,i}^{size}\f$.
  /// \param[in] vrbs VRB bitmap to be converted.
  /// \return A CRB bitmap.
  crb_bitmap vrb_to_crb(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const;

  /// \brief Converts a VRB bitmap to CRB indices.
  ///
  /// \param[in] bwp_start   Lowest CRB index of the BWP relative to CRB0 (PointA) as \f$N_{BWP,i}^{start}\f$.
  /// \param[in] bwp_size    BWP size in CRB as \f$N_{BWP,i}^{size}\f$.
  /// \param[in] vrbs VRB bitmap to be converted.
  /// \return A CRB vector of indices.
  static_vector<uint16_t, MAX_NOF_PRBS>
  vrb_to_crb_indices(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const;

  /// \brief Equal comparison between two non-interleaved VRB-to-PRB mappers.
  bool operator==(const non_interleaved_mapping& other) const { return config == other.config; }
};

/// \brief Describes an interleaved VRB-to-PRB mapping.
class interleaved_mapping
{
private:
  configuration config;

  static_vector<uint16_t, MAX_NOF_PRBS> vrb_to_prb_indices;
  static_vector<uint16_t, MAX_NOF_PRBS> prb_to_vrb_indices;

public:
  interleaved_mapping(const configuration& config_);

  /// \brief Converts a VRB to its corresponding PRB.
  ///
  /// \param[in] vrb VRB to be converted.
  /// \return A PRB.
  unsigned vrb_to_prb(unsigned vrb) const;

  /// \brief Converts a PRB to its corresponding VRB.
  ///
  /// \param[in] prb PRB to be converted.
  /// \return A VRB.
  unsigned prb_to_vrb(unsigned prb) const;

  /// \brief Converts a VRB interval to its corresponding PRB intervals.
  ///
  /// \param[in] vrbs VRB interval to be converted.
  /// \return A pair of PRB intervals.
  std::pair<prb_interval, prb_interval> vrb_to_prb(const vrb_interval& vrbs) const;

  /// \brief Converts a VRB bitmap to a PRB bitmap.
  ///
  /// \param[in] bwp_size Size of the BWP where the PRB bitmap is defined.
  /// \param[in] vrbs VRB bitmap to be converted.
  /// \return A PRB bitmap.
  prb_bitmap vrb_to_prb(unsigned bwp_size, const vrb_bitmap& vrbs) const;

  /// \brief Converts a PRB bitmap to a VRB bitmap.
  ///
  /// \param[in] prbs PRB bitmap to be converted.
  /// \return A VRB bitmap.
  vrb_bitmap prb_to_vrb(const prb_bitmap& prbs) const;

  /// \brief Converts a VRB bitmap to a CRB bitmap.
  ///
  /// \param[in] bwp_start Start of the BWP where the PRB bitmap is defined.
  /// \param[in] bwp_size Size of the BWP where the PRB bitmap is defined.
  /// \param[in] vrbs VRB bitmap to be converted.
  /// \return A CRB bitmap.
  crb_bitmap vrb_to_crb(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const;

  /// \brief Converts a VRB bitmap to CRB indices.
  ///
  /// \param[in] bwp_start Start of the BWP where the PRB bitmap is defined.
  /// \param[in] bwp_size Size of the BWP where the PRB bitmap is defined.
  /// \param[in] vrbs VRB bitmap to be converted.
  /// \return A CRB vector of indices.
  static_vector<uint16_t, MAX_NOF_PRBS>
  vrb_to_crb_indices(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const;

  /// Equal comparison between two interleaved VRB-to-PRB mappers.
  bool operator==(const interleaved_mapping& other) const { return config == other.config; }
};

} // namespace srsran::vrb_to_prb
