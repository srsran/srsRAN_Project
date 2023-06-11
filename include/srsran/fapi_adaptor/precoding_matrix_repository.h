/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/precoding/precoding_configuration.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief Precoding matrix repository.
///
/// The repository stores precoding matrices that are accessible using a precoding matrix index.
class precoding_matrix_repository
{
public:
  explicit precoding_matrix_repository(std::vector<precoding_configuration>&& repo_) : repo(std::move(repo_)) {}

  /// \brief Returns the precoding matrix associated to the given index.
  ///
  /// Index value must be valid, i.e. a precoding configuration must exist in the repository for that index.
  const precoding_configuration& get_precoding_configuration(unsigned index) const;

private:
  std::vector<precoding_configuration> repo;
};

} // namespace fapi_adaptor
} // namespace srsran
