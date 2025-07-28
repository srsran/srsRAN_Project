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

#include "mac_fapi_adaptor_factory_impl.h"
#include "mac_fapi_adaptor_impl.h"
#include "mac_fapi_sector_adaptor_impl.h"
#include "srsran/fapi/decorator.h"
#include "srsran/fapi/decorator_factory.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

/// MAC-FAPI adaptor wrapper that wraps an adaptor and a FAPI decorator.
class mac_fapi_adaptor_wrapper : public mac_fapi_sector_adaptor
{
public:
  mac_fapi_adaptor_wrapper(std::unique_ptr<mac_fapi_sector_adaptor> adaptor_,
                           std::unique_ptr<fapi::fapi_decorator>    decorator_) :
    decorator(std::move(decorator_)), adaptor(std::move(adaptor_))
  {
    srsran_assert(adaptor, "Invalid MAC-FAPI sector adaptor");

    if (decorator) {
      decorator->set_slot_data_message_notifier(adaptor->get_slot_data_message_notifier());
      decorator->set_error_message_notifier(adaptor->get_error_message_notifier());
      decorator->set_slot_time_message_notifier(adaptor->get_slot_time_message_notifier());
    }
  }

  // See interface for documentation.
  fapi::slot_time_message_notifier& get_slot_time_message_notifier() override
  {
    return decorator ? decorator->get_slot_time_message_notifier() : adaptor->get_slot_time_message_notifier();
  }

  // See interface for documentation.
  fapi::error_message_notifier& get_error_message_notifier() override
  {
    return decorator ? decorator->get_error_message_notifier() : adaptor->get_error_message_notifier();
  }

  // See interface for documentation.
  fapi::slot_data_message_notifier& get_slot_data_message_notifier() override
  {
    return decorator ? decorator->get_slot_data_message_notifier() : adaptor->get_slot_data_message_notifier();
  }

  // See interface for documentation.
  mac_cell_result_notifier& get_cell_result_notifier() override { return adaptor->get_cell_result_notifier(); }

  // See interface for documentation.
  void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override
  {
    adaptor->set_cell_slot_handler(mac_slot_handler);
  }

  // See interface for documentation.
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override
  {
    adaptor->set_cell_rach_handler(mac_rach_handler);
  }

  // See interface for documentation.
  void set_cell_pdu_handler(mac_pdu_handler& handler) override { adaptor->set_cell_pdu_handler(handler); }

  // See interface for documentation.
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override
  {
    adaptor->set_cell_crc_handler(handler);
  }

private:
  std::unique_ptr<fapi::fapi_decorator>    decorator;
  std::unique_ptr<mac_fapi_sector_adaptor> adaptor;
};

} // namespace

std::unique_ptr<mac_fapi_adaptor> mac_fapi_adaptor_factory_impl::create(const mac_fapi_adaptor_config&  config,
                                                                        mac_fapi_adaptor_dependencies&& dependencies)
{
  std::vector<std::unique_ptr<mac_fapi_sector_adaptor>> sectors;
  for (unsigned i = 0, e = config.sectors.size(); i != e; ++i) {
    const auto& sector_cfg          = config.sectors[i];
    auto&       sector_dependencies = dependencies.sectors[i];

    srsran_assert(sector_dependencies.gateway, "Invalid FAPI slot message gateway");
    srsran_assert(sector_dependencies.last_msg_notifier, "Invalid FAPI last message notifier");

    // Create FAPI decorators configuration.
    fapi::decorator_config decorator_cfg;
    if (sector_cfg.log_level == srslog::basic_levels::debug) {
      decorator_cfg.logging_cfg.emplace(fapi::logging_decorator_config{i,
                                                                       srslog::fetch_basic_logger("FAPI", true),
                                                                       *sector_dependencies.gateway,
                                                                       *sector_dependencies.last_msg_notifier});
    }
    if (sector_cfg.l2_nof_slots_ahead != 0) {
      srsran_assert(sector_dependencies.bufferer_task_executor,
                    "Invalid executor for the FAPI message bufferer decorator");
      decorator_cfg.bufferer_cfg.emplace(
          fapi::message_bufferer_decorator_config{i,
                                                  sector_cfg.l2_nof_slots_ahead,
                                                  sector_cfg.scs,
                                                  *sector_dependencies.bufferer_task_executor.value(),
                                                  *sector_dependencies.gateway,
                                                  *sector_dependencies.last_msg_notifier});
    }
    auto decorators = fapi::create_decorators(decorator_cfg);

    // Update the gateway and last message notifier to the decorators if they are present.
    if (decorators) {
      sector_dependencies.gateway           = &decorators->get_slot_message_gateway();
      sector_dependencies.last_msg_notifier = &decorators->get_slot_last_message_notifier();
    }

    auto sector_adaptor = std::make_unique<mac_fapi_sector_adaptor_impl>(sector_cfg, std::move(sector_dependencies));

    sectors.push_back(std::make_unique<mac_fapi_adaptor_wrapper>(std::move(sector_adaptor), std::move(decorators)));
  }

  return std::make_unique<mac_fapi_adaptor_impl>(std::move(sectors));
}

std::unique_ptr<mac_fapi_adaptor_factory> srsran::fapi_adaptor::create_mac_fapi_adaptor_factory()
{
  return std::make_unique<mac_fapi_adaptor_factory_impl>();
}
