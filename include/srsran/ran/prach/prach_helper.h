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

#include "srsran/adt/expected.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/rach_config_common.h"
#include <optional>

namespace srsran {

struct tdd_ul_dl_config_common;

namespace prach_helper {

/// \brief Checks whether a given PRACH configuration index is valid for a given frequency range and duplex mode.
/// \return \c default_success_t if the given parameters are supported, otherwise an error message is provided.
error_type<std::string> prach_config_index_is_valid(uint8_t prach_cfg_idx, frequency_range fr, duplex_mode dplx_mode);

/// \brief Checks whether a given PRACH Zero Correlation Zone (ZCZ) is valid for a given PRACH config index, frequency
/// range, and duplex mode.
/// \return \c default_success_t if the given parameters are supported, otherwise an error message is provided.
error_type<std::string> zero_correlation_zone_is_valid(uint8_t         zero_correlation_zone,
                                                       uint8_t         prach_cfg_idx,
                                                       frequency_range freq_range,
                                                       duplex_mode     dplx_mode);

/// \brief Finds whether a PRACH config index fits in the provided TDD pattern.
/// \return In case of failure, returns the range of PRACH slots that did not fit in the TDD pattern. If the PRACH
/// configuration is invalid, an empty interval of slots is returned.
error_type<interval<uint8_t>>
prach_fits_in_tdd_pattern(subcarrier_spacing pusch_scs, uint8_t prach_cfg_idx, const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Finds a PRACH configuration index that ensures that PRACH falls in an TDD UL slot.
std::optional<uint8_t> find_valid_prach_config_index(subcarrier_spacing             pusch_scs,
                                                     uint8_t                        zero_correlation_zone,
                                                     const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Checks whether the nof. SSB per RACH occasion and nof. contention based preambles per SSB is valid.
/// \return In case config is not supported, a string with an error message is returned.
error_type<std::string> nof_ssb_per_ro_and_nof_cb_preambles_per_ssb_is_valid(ssb_per_rach_occasions nof_ssb_per_ro,
                                                                             uint8_t nof_cb_preambles_per_ssb);

/// \brief Checks whether a given PRACH Root Sequence Index is valid for a given PRACH format.
/// \return \c default_success_t if the given parameters are supported, otherwise an error message is provided.
error_type<std::string> prach_root_sequence_index_is_valid(unsigned prach_root_seq_idx, prach_format_type format);
} // namespace prach_helper
} // namespace srsran
