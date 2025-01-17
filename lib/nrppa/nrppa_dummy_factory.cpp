/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "nrppa_dummy_impl.h"
#include "srsran/nrppa/nrppa_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<nrppa_interface> srsran::srs_cu_cp::create_nrppa(const cu_cp_configuration& cfg,
                                                                 nrppa_cu_cp_notifier&      cu_cp_notifier,
                                                                 common_task_scheduler&     common_task_sched)
{
  (void)&cfg;
  (void)&cu_cp_notifier;
  auto nrppa = std::make_unique<nrppa_dummy_impl>();
  return nrppa;
}
