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
/// \brief Detector interface for short blocks.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/modulation_scheme.h"

#include <memory>

namespace srsran {

/// Short-block detector interface.
class short_block_detector
{
public:
  /// Default destructor.
  virtual ~short_block_detector() = default;

  /// \brief Detection, decoding and rate-dematching of short blocks.
  ///
  /// Checks whether the provided signal contains a codeblock and, if so, decodes its content assuming the encoding
  /// followed TS38.212 Section 5.3.3 and the rate-matching scheme of TS38.212 Section 5.4.3.
  /// \param[out] output Detected message (one bit per entry).
  /// \param[in]  input  Received codeblock (log-likelihood ratios).
  /// \param[in]  mod    Modulation scheme.
  /// \return True if \c input passes a detection test.
  /// \remark When the detection test fails, the function returns \c false and the content of \c output is unreliable.
  virtual bool detect(span<uint8_t> output, span<const log_likelihood_ratio> input, modulation_scheme mod) = 0;
};
} // namespace srsran
