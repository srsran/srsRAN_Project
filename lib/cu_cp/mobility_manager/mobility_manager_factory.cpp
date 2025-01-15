/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mobility_manager_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<mobility_manager>
srsran::srs_cu_cp::create_mobility_manager(const mobility_manager_cfg&      cfg,
                                           mobility_manager_cu_cp_notifier& cu_cp_notifier,
                                           ngap_repository&                 ngap_db,
                                           du_processor_repository&         du_db,
                                           ue_manager&                      ue_mng)
{
  auto mobility_mng = std::make_unique<mobility_manager>(cfg, cu_cp_notifier, ngap_db, du_db, ue_mng);
  return mobility_mng;
}
