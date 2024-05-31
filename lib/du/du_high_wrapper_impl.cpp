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

#include "du_high_wrapper_impl.h"

using namespace srsran;

namespace {
/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  std::vector<std::reference_wrapper<mac_cell_result_notifier>> cells;

public:
  explicit phy_dummy(std::vector<std::reference_wrapper<mac_cell_result_notifier>> cells_) : cells(std::move(cells_)) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override
  {
    return cells[static_cast<unsigned>(cell_index)].get();
  }
};

} // namespace

du_high_wrapper_impl::du_high_wrapper_impl(du_high_wrapper_impl_dependencies&& du_dependencies) :
  logger(*du_dependencies.logger),
  du_high_adaptor(std::move(du_dependencies.du_high_adaptor)),
  du_high_result_notifier([](span<std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>> fapi_adaptors) {
    std::vector<std::reference_wrapper<mac_cell_result_notifier>> cells;
    for (unsigned i = 0, e = fapi_adaptors.size(); i != e; ++i) {
      cells.push_back(std::ref(fapi_adaptors[i]->get_cell_result_notifier()));
    }

    return std::make_unique<phy_dummy>(std::move(cells));
  }(du_high_adaptor))
{
  srsran_assert(!du_high_adaptor.empty(), "Invalid FAPI MAC adaptor");
}

void du_high_wrapper_impl::start()
{
  srsran_assert(du_hi, "Invalid DU high");

  logger.info("Starting DU...");
  du_hi->start();

  // Configure the FAPI -> DU interface.
  for (unsigned i = 0, e = du_high_adaptor.size(); i != e; ++i) {
    du_cell_index_t cell_id = to_du_cell_index(i);
    du_high_adaptor[i]->set_cell_slot_handler(du_hi->get_slot_handler(cell_id));
    du_high_adaptor[i]->set_cell_rach_handler(du_hi->get_rach_handler(cell_id));
    du_high_adaptor[i]->set_cell_pdu_handler(du_hi->get_pdu_handler());
    du_high_adaptor[i]->set_cell_crc_handler(du_hi->get_control_info_handler(cell_id));
  }

  logger.info("DU started successfully");
}

void du_high_wrapper_impl::stop()
{
  srsran_assert(du_hi, "Invalid DU high");

  logger.info("Stopping DU...");
  du_hi->stop();
  logger.info("DU stopped successfully");
}

du_high& du_high_wrapper_impl::get_du_high()
{
  return *du_hi;
}

void du_high_wrapper_impl::set_du_high(std::unique_ptr<du_high> updated_du_high)
{
  du_hi = std::move(updated_du_high);

  srsran_assert(du_hi, "Invalid DU high");
}

fapi::slot_data_message_notifier& du_high_wrapper_impl::get_slot_data_message_notifier(unsigned cell_id)
{
  srsran_assert(cell_id < du_high_adaptor.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                du_high_adaptor.size());

  return du_high_adaptor[cell_id]->get_slot_data_notifier();
}

fapi::slot_error_message_notifier& du_high_wrapper_impl::get_slot_error_message_notifier(unsigned cell_id)
{
  srsran_assert(cell_id < du_high_adaptor.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                du_high_adaptor.size());

  return du_high_adaptor[cell_id]->get_slot_error_notifier();
}

fapi::slot_time_message_notifier& du_high_wrapper_impl::get_slot_time_message_notifier(unsigned cell_id)
{
  srsran_assert(cell_id < du_high_adaptor.size(),
                "Invalid cell index '{}'. Valid cell indexes [0-{})",
                cell_id,
                du_high_adaptor.size());

  return du_high_adaptor[cell_id]->get_slot_time_notifier();
}
