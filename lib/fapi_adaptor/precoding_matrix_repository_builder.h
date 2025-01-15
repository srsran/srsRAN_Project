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
