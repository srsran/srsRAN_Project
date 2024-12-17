/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_o_du_impl.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

split6_o_du_impl::split6_o_du_impl(std::vector<std::unique_ptr<fapi::fapi_adaptor>> adaptors_,
                                   std::unique_ptr<srs_du::o_du_high>               odu_hi_) :
  odu_hi(std::move(odu_hi_)), adaptors(std::move(adaptors_))
{
  srsran_assert(odu_hi, "Invalid O-DU high");
  srsran_assert(adaptors.size(), "Invalid FAPI adaptor");

  // Hardcoded 0 is because at this moment there is one DU instance with one cell per cell, so we can always access the
  // cell id 0 of the DU.
  for (unsigned i = 0, e = adaptors.size(); i != e; ++i) {
    adaptors[i]->get_message_interface_collection().set_slot_data_message_notifier(
        odu_hi->get_slot_data_message_notifier(i));
    adaptors[i]->get_message_interface_collection().set_slot_error_message_notifier(
        odu_hi->get_slot_error_message_notifier(i));
    adaptors[i]->get_message_interface_collection().set_slot_time_message_notifier(
        odu_hi->get_slot_time_message_notifier(i));
  }
}

void split6_o_du_impl::start()
{
  odu_hi->get_power_controller().start();

  for (auto& adaptor : adaptors) {
    adaptor->get_power_operation_controller().start();
  }
}

void split6_o_du_impl::stop()
{
  odu_hi->get_power_controller().stop();

  for (auto& adaptor : adaptors) {
    adaptor->get_power_operation_controller().stop();
  }
}
