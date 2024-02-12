/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/rrc/rrc_du_factory.h"
#include "rrc_du_impl.h"
#include "ue/rrc_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<rrc_du_interface> srsran::srs_cu_cp::create_rrc_du(const rrc_du_creation_message& msg)
{
  return std::make_unique<rrc_du_impl>(
      msg.cfg, msg.rrc_ue_du_proc_notifier, msg.nas_notifier, msg.ngap_ctrl_notifier, msg.rrc_du_cu_cp_notifier);
}