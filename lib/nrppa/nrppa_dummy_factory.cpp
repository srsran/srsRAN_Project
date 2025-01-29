/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
