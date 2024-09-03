/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/rrc/rrc_du_factory.h"
#include "rrc_du_impl.h"
#include "ue/rrc_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<rrc_du> srsran::srs_cu_cp::create_rrc_du(const rrc_du_creation_message& msg)
{
  return std::make_unique<rrc_du_impl>(msg.cfg, msg.rrc_du_cu_cp_notifier);
}
