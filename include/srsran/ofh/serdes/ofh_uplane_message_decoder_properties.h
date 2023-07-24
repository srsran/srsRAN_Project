/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/complex.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/serdes/ofh_uplane_message_properties.h"
#include "srsran/ran/resource_block.h"

namespace srsran {
namespace ofh {

/// Maximum number of supported sections.
static constexpr unsigned MAX_NOF_SUPPORTED_SECTIONS = 2U;

/// Open Fronthaul User-Plane section parameters.
struct uplane_section_params {
  /// Section identifier.
  unsigned section_id;
  /// Resource block indicator.
  bool is_every_rb_used;
  /// Symbol number increment command.
  bool use_current_symbol_number;
  /// Start PRB.
  unsigned start_prb;
  /// Number of PRBs.
  unsigned nof_prbs;
  /// User data compression header.
  ru_compression_params ud_comp_hdr;
  /// User data compression length.
  optional<unsigned> ud_comp_len;
  /// User data compression parameter.
  /// \note For simplicity, all the PRBs use the same compression parameters.
  optional<unsigned> ud_comp_param;
  /// IQ samples.
  static_vector<cf_t, MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB> iq_samples;
};

/// Open Fronthaul User-Plane message decoder results.
struct uplane_message_decoder_results {
  /// Open Fronthaul User-Plane message parameters.
  uplane_message_params params;
  /// User-Plane message sections.
  static_vector<uplane_section_params, MAX_NOF_SUPPORTED_SECTIONS> sections;
};

} // namespace ofh
} // namespace srsran
