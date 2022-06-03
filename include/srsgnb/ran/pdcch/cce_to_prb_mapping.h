/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_PDCCH_CCE_TO_PRB_MAPPING_H
#define SRSGNB_RAN_PDCCH_CCE_TO_PRB_MAPPING_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/pdcch/coreset.h"
#include "srsgnb/ran/pdcch/pdcch_constants.h"

namespace srsgnb {

/// Describes a PRB index list of unsigned 16 bit indicating the PRB index.
using prb_index_list = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;

/// \brief Calculates the PDCCH CCE to PRB mapping for a PDCCH transmission in CORESET0.
///
/// \param[in] N_coreset0_start  The lowest RB index of the CORESET0 relative to CRB0 (PointA).
/// \param[in] N_coreset0_size   The CORESET0 bandwidth in RBs.
/// \param[in] N_symb_coreset    The number of symbols for the PDCCH transmission.
/// \param[in] N_id_cell         Physical cell identifier.
/// \param[in] aggregation_level Number of CCE used for the PDCCH transmission.
/// \param[in] cce_index         Initial CCE index for the PDCCH transmision.
/// \return A list of the resource blocks used by the PDCCH transmission.
prb_index_list cce_to_prb_mapping_coreset0(unsigned N_coreset0_start,
                                           unsigned N_coreset0_size,
                                           unsigned N_symb_coreset,
                                           unsigned N_id_cell,
                                           unsigned aggregation_level,
                                           unsigned cce_index);

/// \brief Calculates the PDCCH CCE to PRB mapping for a non-interleaved PDCCH transmission.
///
/// \param[in] N_bwp_start       Start of the BWP relative to CRB0 (PointA).
/// \param[in] freq_resources    Indicates the frequency resources active for the CORESET.
/// \param[in] N_symb_coreset    The number of symbols for the PDCCH transmission.
/// \param[in] aggregation_level Number of CCE used for the PDCCH transmission.
/// \param[in] cce_index         Initial CCE index for the PDCCH transmision.
/// \return A list of the resource blocks used by the PDCCH transmission.
prb_index_list cce_to_prb_mapping_non_interleaved(unsigned                    N_bwp_start,
                                                  const freq_resource_bitmap& freq_resources,
                                                  unsigned                    N_symb_coreset,
                                                  unsigned                    aggregation_level,
                                                  unsigned                    cce_index);

/// \brief Calculates the PDCCH CCE to PRB mapping for an interleaved PDCCH transmission.
///
/// \param[in] N_bwp_start       Start of the BWP relative to CRB0 (PointA).
/// \param[in] freq_resources    Indicates the frequency resources active for the CORESET.
/// \param[in] N_symb_coreset    The number of symbols for the PDCCH transmission.
/// \param[in] reg_bundle_size   Parameter \c reg-BundleSize.
/// \param[in] interleaver_size  Parameter \c interleaverSize.
/// \param[in] shift_index       Parameter \c shiftIndex if available, otherwise \f$N_{ID}^{cell}\f$.
/// \param[in] aggregation_level Number of CCE used for the PDCCH transmission.
/// \param[in] cce_index         Initial CCE index for the PDCCH transmision.
/// \return A list of the resource blocks used by the PDCCH transmission.
prb_index_list cce_to_prb_mapping_interleaved(unsigned                    N_bwp_start,
                                              const freq_resource_bitmap& freq_resources,
                                              unsigned                    N_symb_coreset,
                                              unsigned                    reg_bundle_size,
                                              unsigned                    interleaver_size,
                                              unsigned                    shift_index,
                                              unsigned                    aggregation_level,
                                              unsigned                    cce_index);

} // namespace srsgnb

#endif // SRSGNB_RAN_PDCCH_CCE_TO_PRB_MAPPING_H
