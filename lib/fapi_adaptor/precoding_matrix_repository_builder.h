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

#include "srsran/fapi_adaptor/precoding_matrix_repository.h"
#include <memory>

namespace srsran {
namespace fapi_adaptor {

/// Precoding matrix repository builder.
class precoding_matrix_repository_builder
{
public:
  explicit precoding_matrix_repository_builder(unsigned size) { repository.reserve(size); }

  /// Adds the given precoding configuration to the repository with the given index.
  void add(unsigned index, const precoding_weight_matrix& precoding)
  {
    if (index >= repository.size()) {
      repository.resize(index + 1U);
    }

    repository[index] = precoding;
  }

  /// Builds and returns a precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> build()
  {
    return std::make_unique<precoding_matrix_repository>(std::move(repository));
  }

private:
  std::vector<precoding_weight_matrix> repository;
};

} // namespace fapi_adaptor
} // namespace srsran
