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

#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/e2sm/e2sm.h"

using namespace asn1::e2ap;
using namespace srsran;

e2sm_manager::e2sm_manager(srslog::basic_logger& logger_) : logger(logger_) {}

void e2sm_manager::add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface)
{
  e2sm_iface_list.emplace(oid, std::move(e2sm_iface));
}

e2sm_interface* e2sm_manager::get_e2sm_interface(std::string oid)
{
  auto it = e2sm_iface_list.find(oid);
  if (it == e2sm_iface_list.end()) {
    logger.error("OID not supported");
    return nullptr;
  }
  return &(*(it->second));
}

e2sm_interface* e2sm_manager::get_e2sm_interface(uint16_t ran_func_id)
{
  auto it = supported_ran_functions.find(ran_func_id);
  if (it == supported_ran_functions.end()) {
    logger.error("RAN function ID not supported");
    return nullptr;
  }
  return get_e2sm_interface(it->second);
}

void e2sm_manager::add_supported_ran_function(uint16_t ran_func_id, std::string oid)
{
  if (e2sm_iface_list.count(oid)) {
    supported_ran_functions.emplace(ran_func_id, oid);
  } else {
    logger.error("OID/RAN function ID not supported");
  }
}