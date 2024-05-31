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

#include "du_low_wrapper_impl.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

du_low_wrapper_impl::du_low_wrapper_impl(std::unique_ptr<du_low>                                      du_lo_,
                                         std::vector<std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>> fapi_adaptors_) :
  du_lo(std::move(du_lo_)), fapi_adaptors(std::move(fapi_adaptors_))
{
  srsran_assert(du_lo, "Invalid upper PHY");
  srsran_assert(!fapi_adaptors.empty(), "Invalid FAPI adaptor");

  for (unsigned i = 0, e = fapi_adaptors.size(); i != e; ++i) {
    du_lo->get_upper_phy(i).set_rx_results_notifier(fapi_adaptors[i]->get_rx_results_notifier());
    du_lo->get_upper_phy(i).set_timing_notifier(fapi_adaptors[i]->get_timing_notifier());
    du_lo->get_upper_phy(i).set_error_notifier(fapi_adaptors[i]->get_error_notifier());
  }
}

du_low& du_low_wrapper_impl::get_du_low()
{
  return *du_lo;
}

fapi::slot_message_gateway& du_low_wrapper_impl::get_slot_message_gateway(unsigned cell_id)
{
  srsran_assert(cell_id < fapi_adaptors.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                fapi_adaptors.size());

  return fapi_adaptors[cell_id]->get_slot_message_gateway();
}

fapi::slot_last_message_notifier& du_low_wrapper_impl::get_slot_last_message_notifier(unsigned cell_id)
{
  srsran_assert(cell_id < fapi_adaptors.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                fapi_adaptors.size());

  return fapi_adaptors[cell_id]->get_slot_last_message_notifier();
}

void du_low_wrapper_impl::set_slot_time_message_notifier(unsigned                          cell_id,
                                                         fapi::slot_time_message_notifier& fapi_time_notifier)
{
  srsran_assert(cell_id < fapi_adaptors.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                fapi_adaptors.size());

  fapi_adaptors[cell_id]->set_slot_time_message_notifier(fapi_time_notifier);
}

void du_low_wrapper_impl::set_slot_error_message_notifier(unsigned                           cell_id,
                                                          fapi::slot_error_message_notifier& fapi_error_notifier)
{
  srsran_assert(cell_id < fapi_adaptors.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                fapi_adaptors.size());

  fapi_adaptors[cell_id]->set_slot_error_message_notifier(fapi_error_notifier);
}

void du_low_wrapper_impl::set_slot_data_message_notifier(unsigned                          cell_id,
                                                         fapi::slot_data_message_notifier& fapi_data_notifier)
{
  srsran_assert(cell_id < fapi_adaptors.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                fapi_adaptors.size());

  fapi_adaptors[cell_id]->set_slot_data_message_notifier(fapi_data_notifier);
}
