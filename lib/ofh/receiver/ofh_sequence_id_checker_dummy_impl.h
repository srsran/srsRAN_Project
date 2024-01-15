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

#include "ofh_sequence_id_checker.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul sequence identifier checker dummy implementation.
class sequence_id_checker_dummy_impl : public sequence_id_checker
{
public:
  // See interface for documentation.
  int update_and_compare_seq_id(unsigned eaxc, uint8_t seq_id) override { return 0; };
};

} // namespace ofh
} // namespace srsran
