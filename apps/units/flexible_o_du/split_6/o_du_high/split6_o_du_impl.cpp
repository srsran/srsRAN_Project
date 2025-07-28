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

#include "split6_o_du_impl.h"
#include "srsran/fapi_adaptor/fapi_operation_controller.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_adaptor.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

split6_o_du_impl::split6_o_du_impl(std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors_,
                                   std::unique_ptr<srs_du::o_du_high>               odu_hi_) :
  odu_hi(std::move(odu_hi_)), adaptors(std::move(adaptors_))
{
  srsran_assert(odu_hi, "Invalid O-DU high");
  srsran_assert(adaptors.size(), "Invalid FAPI adaptor");

  for (unsigned i = 0, e = adaptors.size(); i != e; ++i) {
    fapi_adaptor::mac_fapi_sector_adaptor& fapi_sector = odu_hi->get_mac_fapi_adaptor().get_sector_adaptor(i);

    adaptors[i]->get_message_interface_collection().set_slot_data_message_notifier(
        fapi_sector.get_slot_data_message_notifier());
    adaptors[i]->get_message_interface_collection().set_error_message_notifier(
        fapi_sector.get_error_message_notifier());
    adaptors[i]->get_message_interface_collection().set_slot_time_message_notifier(
        fapi_sector.get_slot_time_message_notifier());
  }
}

void split6_o_du_impl::start()
{
  odu_hi->get_operation_controller().start();

  for (auto& adaptor : adaptors) {
    adaptor->get_operation_controller().start();
  }
}

void split6_o_du_impl::stop()
{
  odu_hi->get_operation_controller().stop();

  for (auto& adaptor : adaptors) {
    adaptor->get_operation_controller().stop();
  }
}
