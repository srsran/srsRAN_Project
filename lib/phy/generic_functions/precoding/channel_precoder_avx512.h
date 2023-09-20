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

/// \file
/// \brief Channel precoder AVX512 implementation.

#pragma once

#include "channel_precoder_impl.h"

namespace srsran {

/// Channel precoder implementation.
class channel_precoder_avx512 : public channel_precoder_impl
{
  // See interface for documentation.
  void
  apply_precoding_port(span<cf_t> port_re, const re_buffer_reader& input_re, span<const cf_t> port_weights) override;

public:
  // See interface for documentation.
  void apply_layer_map_and_precoding(re_buffer_writer&              output,
                                     span<const ci8_t>              input,
                                     const precoding_weight_matrix& precoding) override;
};

} // namespace srsran
