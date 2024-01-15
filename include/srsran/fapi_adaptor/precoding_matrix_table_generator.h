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

#include "srsran/fapi_adaptor/precoding_matrix_mapper.h"
#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include <memory>
#include <tuple>

namespace srsran {
namespace fapi_adaptor {

/// Generates the precoding matrix mapper and precoding matrix repository for the given number of antenna ports.
std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_matrix_repository>>
generate_precoding_matrix_tables(unsigned nof_antenna_ports);

} // namespace fapi_adaptor
} // namespace srsran
