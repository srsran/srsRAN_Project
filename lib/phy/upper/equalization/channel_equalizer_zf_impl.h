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

/// \file
/// \brief Channel equalizer implementation using the Zero Forcing method.

#pragma once

#include "srsran/phy/upper/equalization/channel_equalizer.h"

namespace srsran {

/// Channel equalizer implementation using the Zero Forcing algorithm.
class channel_equalizer_zf_impl : public channel_equalizer
{
public:
  /// Default constructor.
  explicit channel_equalizer_zf_impl() = default;

  // See interface for documentation.
  void equalize(re_list&           eq_symbols,
                noise_var_list&    eq_noise_vars,
                const re_list&     ch_symbols,
                const ch_est_list& ch_estimates,
                span<const float>  noise_var_estimates,
                float              tx_scaling) override;
};

} // namespace srsran
