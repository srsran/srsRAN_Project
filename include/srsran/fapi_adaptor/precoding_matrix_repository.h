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

#include "srsran/ran/precoding/precoding_weight_matrix.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief Precoding matrix repository.
///
/// The repository stores precoding matrices that are accessible using a precoding matrix index.
class precoding_matrix_repository
{
public:
  explicit precoding_matrix_repository(std::vector<precoding_weight_matrix>&& repo_) : repo(std::move(repo_))
  {
    srsran_assert(!repo.empty(), "Empty container");
  }

  /// \brief Returns the precoding matrix associated to the given index.
  ///
  /// Index value must be valid, i.e. a precoding configuration must exist in the repository for that index.
  const precoding_weight_matrix& get_precoding_matrix(unsigned index) const;

private:
  std::vector<precoding_weight_matrix> repo;
};

} // namespace fapi_adaptor
} // namespace srsran
