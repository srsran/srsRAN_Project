/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/phy/constants.h"
#include <cstdint>

namespace srsran {

/// \brief Describes a VRB-to-PRB mapping.
///
/// This class abstracts the virtual to physical resource mapping for PDSCH.
///
/// The default constructor is equivalent to use create_non_interleaved_other().
class vrb_to_prb_mapper
{
private:
  /// Indicates VRB index of the lowest VRB in the CORESET used for the DCI transmission.
  unsigned coreset_start = 0;
  /// Indicates the number of RB bundles \f$N_{bundle}\f$. It is set to zero for non-interleaved mapping.
  unsigned nof_bundles = 0;
  /// Indicates the first RB bundle size.
  unsigned first_bundle_size = 0;
  /// Indicates the last RB bundle size.
  unsigned last_bundle_size = 0;
  /// Indicates the other RB bundle sizes.
  unsigned other_bundle_size = 0;

public:
  /// \brief Creates a non-interleaved VRB-to-PRB mapper for PDSCH transmissions scheduled by DCI Format 1_0 in common
  /// SS.
  ///
  /// Implemented as per TS38.211 Section 7.3.1.6 Case 1:
  /// - A virtual resource \f$n\f$ is mapped to the physical block \f$n + N_{start}^{CORESET}\f$.
  ///
  /// If the PDSCH is scheduled with DCI format 1_0 with the CRC scrambled by SI-RNTI in Type0-PDCCH common search space
  /// in CORESET 0, the BWP start and size must match with the CORESET0 start and size. Consequently,
  /// \f$N_{start}^{CORESET}\f$ must be zero.
  ///
  /// \param[in] N_start_coreset VRB index of the lowest-numbered RB in the CORESET used for the DCI transmission as
  /// \f$N_{start}^{CORESET}\f$.
  /// \return A VRB-to-PRB mapper instance.
  static vrb_to_prb_mapper create_non_interleaved_common_ss(unsigned N_start_coreset);

  /// \brief Creates a non-interleaved VRB-to-PRB mapper for PDSCH transmissions scheduled by DCI other than Format 1_0
  /// in common SS.
  ///
  /// Implemented as per TS38.211 Section 7.3.1.6 Case 2:
  /// - A virtual resouce \f$n\f$ is mapped to the physical block \f$n\f$.
  ///
  /// \return A VRB-to-PRB mapper instance.
  static vrb_to_prb_mapper create_non_interleaved_other();

  /// \brief Creates a VRB-to-PRB mapper for PDSCH transmissions scheduled with DCI Format 1_0 with the CRC scrambled
  /// by SI-RNTI in Type0-PDCCH common SS in CORESET0.
  ///
  /// Implemented as per TS38.211 Section 7.3.1.6 Case 3:
  /// - the number of bundles is calculated as \f$N_{bundle}=\lceil N_{BWP,init}^{size}/L \rceil\f$,
  /// - the last RB bundle consists of \f$N_{BWP,init}^{size} \bmod L\f$ if \f$N_{BWP,init}^{size} \bmod L > 0\f$,
  /// - all other resource block bundles consist of \f$L\f$ RBs, and
  /// - the bundle size is \f$L=2\f$.
  ///
  ///
  /// \param[in] N_start_coreset VRB index of the lowest-numbered RB in the CORESET used for the DCI transmission as
  /// \f$N_{start}^{CORESET}\f$.
  /// \param[in] N_bwp_init_size The size in RBs of CORESET0 as \f$N_{BWP,init}^{size}\f$.
  /// \return A VRB-to-PRB mapper instance.
  static vrb_to_prb_mapper create_interleaved_coreset0(unsigned N_start_coreset, unsigned N_bwp_init_size);

  /// \brief Creates a VRB-to-PRB mapper for PDSCH transmissions scheduled by DCI format 1_0 in any common SS except
  /// Type0-PDCCH common SS in CORESET0.
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
  /// \param[in] N_start_coreset VRB index of the lowest-numbered RB in the CORESET used for the DCI transmission as
  /// \f$N_{start}^{CORESET}\f$.
  /// \param[in] N_bwp_i_start BWP \f$i\f$ starting position \f$N_{BWP,i}^{start}\f$.
  /// \param[in] N_bwp_init_size The size in RBs of CORESET0 if available, otherwise the initial bandwidth part size as
  /// \f$N_{BWP,init}^{size}\f$.
  /// \return A VRB-to-PRB mapper instance.
  static vrb_to_prb_mapper
  create_interleaved_common(unsigned N_start_coreset, unsigned N_bwp_i_start, unsigned N_bwp_init_size);

  /// \brief Creates a VRB-to-PRB mapper for all other PDSCH transmissions with interleaved mapping.
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
  /// \return A VRB-to-PRB mapper instance.
  static vrb_to_prb_mapper create_interleaved_other(unsigned N_bwp_i_start, unsigned N_bwp_i_size, unsigned L_i);

  /// \brief Checks whether the resource allocation is interleaved.
  ///
  /// The resource allocation is interleaved if the number of bundles is not zero.
  ///
  /// \return True if the resource allocation is interleaved. Otherwise, False.
  bool is_interleaved() const { return nof_bundles != 0; }

  /// \brief Gets the CORESET start VRB index.
  ///
  /// VRB index of the lowest-numbered RB in the CORESET used for the DCI transmission.
  ///
  /// \return A VRB index if the PDSCH transmission is scheduled by a DCI in a common SS. Otherwise, 0.
  unsigned get_coreset_start() const { return coreset_start; }

  /// \brief Calculates a PDSCH allocation PRB indices.
  ///
  /// \param[in] bwp_size Indicates the BWP size.
  /// \return Return the PRB indices relative to the lowest PRB of the BWP.
  static_vector<uint16_t, MAX_RB> get_allocation_indices(unsigned bwp_size) const;

  /// \brief Equal comparison between two VRB-to-PRB mappers.
  ///
  /// Two mappers are considered equal if  their CORESET start, number of bundles and RB bundle sizes are equal.
  bool operator==(const vrb_to_prb_mapper& other) const
  {
    return (coreset_start == other.coreset_start) && (nof_bundles == other.nof_bundles) &&
           (first_bundle_size == other.first_bundle_size) && (last_bundle_size == other.last_bundle_size) &&
           (other_bundle_size == other.other_bundle_size);
  }
};
} // namespace srsran
