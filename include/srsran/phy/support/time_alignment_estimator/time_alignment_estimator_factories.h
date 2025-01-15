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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator.h"
#include <memory>

namespace srsran {

/// Time alignment estimator factory interface.
class time_alignment_estimator_factory
{
public:
  /// Default destructor.
  virtual ~time_alignment_estimator_factory() = default;

  /// Creates a time alignment estimator.
  virtual std::unique_ptr<time_alignment_estimator> create() = 0;
};

/// Creates a time alignment estimator factory based on DFT.
std::shared_ptr<time_alignment_estimator_factory>
create_time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory);

} // namespace srsran
