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

#include "split6_du_impl.h"

#include "srsran/support/srsran_assert.h"

using namespace srsran;

split6_du_impl::split6_du_impl(std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors_,
                               std::vector<std::unique_ptr<du_high_wrapper>>    du_list_) :
  du_list(std::move(du_list_)), adaptors(std::move(adaptors_))
{
  srsran_assert(!du_list.empty(), "Invalid DU high wrapper");
  srsran_assert(adaptors.size() == du_list.size(), "Number of FAPI adaptors does not match number of DUs");

  // Hardcoded 0 is because at this moment there is one DU instance with one cell per cell, so we can always access the
  // cell id 0 of the DU.
  for (unsigned i = 0, e = du_list.size(); i != e; ++i) {
    adaptors[i]->get_message_interface_collection().set_slot_data_message_notifier(
        du_list[i]->get_slot_data_message_notifier(0));
    adaptors[i]->get_message_interface_collection().set_slot_error_message_notifier(
        du_list[i]->get_slot_error_message_notifier(0));
    adaptors[i]->get_message_interface_collection().set_slot_time_message_notifier(
        du_list[i]->get_slot_time_message_notifier(0));
  }
}

void split6_du_impl::start()
{
  for (auto& du_obj : du_list) {
    du_obj->get_power_controller().start();
  }

  for (auto& adaptor : adaptors) {
    adaptor->get_power_operation_controller().start();
  }
}

void split6_du_impl::stop()
{
  for (auto& du_obj : du_list) {
    du_obj->get_power_controller().stop();
  }

  for (auto& adaptor : adaptors) {
    adaptor->get_power_operation_controller().stop();
  }
}
