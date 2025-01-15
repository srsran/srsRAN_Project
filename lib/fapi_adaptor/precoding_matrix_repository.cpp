/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi_adaptor/precoding_matrix_repository.h"

using namespace srsran;
using namespace fapi_adaptor;

const precoding_weight_matrix& precoding_matrix_repository::get_precoding_matrix(unsigned index) const
{
  srsran_assert(index < repo.size(), "Invalid precoding matrix index={}, repository size={}", index, repo.size());
  srsran_assert(repo[index].get_nof_layers() != 0, "Invalid precoding matrix index={}", index);

  return repo[index];
}
