/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
