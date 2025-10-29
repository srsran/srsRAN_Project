/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_p7_sector_fastpath_adaptor_factory_impl.h"
#include "mac_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "srsran/fapi/decorator.h"
#include "srsran/fapi/decorator_factory.h"
#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor_config.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

/// MAC-FAPI adaptor wrapper that wraps an adaptor and a FAPI decorator.
class mac_fapi_adaptor_wrapper : public mac_fapi_p7_sector_fastpath_adaptor
{
public:
  mac_fapi_adaptor_wrapper(std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> adaptor_,
                           std::unique_ptr<fapi::fapi_decorator>                decorator_) :
    decorator(std::move(decorator_)), adaptor(std::move(adaptor_))
  {
    srsran_assert(adaptor, "Invalid MAC-FAPI sector adaptor");

    if (decorator) {
      decorator->set_slot_data_message_notifier(adaptor->get_slot_data_message_notifier());
      decorator->set_error_message_notifier(adaptor->get_error_message_notifier());
      decorator->set_slot_time_message_notifier(adaptor->get_slot_time_message_notifier());
    }
  }

  void stop() override { adaptor->stop(); }

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
  std::unique_ptr<fapi::fapi_decorator>                decorator;
  std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> adaptor;
};

} // namespace

std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> srsran::fapi_adaptor::create_mac_fapi_p7_sector_fastpath_adaptor(
    const mac_fapi_p7_sector_fastpath_adaptor_config& config,
    mac_fapi_p7_sector_fastpath_adaptor_dependencies  dependencies)
{
  // Create FAPI decorators configuration.
  fapi::decorator_config decorator_cfg;
  if (config.log_level == srslog::basic_levels::debug) {
    decorator_cfg.logging_cfg.emplace(fapi::logging_decorator_config{config.sector_id,
                                                                     srslog::fetch_basic_logger("FAPI", true),
                                                                     dependencies.gateway,
                                                                     dependencies.last_msg_notifier});
  }
  if (config.l2_nof_slots_ahead != 0) {
    srsran_assert(dependencies.bufferer_task_executor, "Invalid executor for the FAPI message bufferer decorator");
    decorator_cfg.bufferer_cfg.emplace(fapi::message_bufferer_decorator_config{config.sector_id,
                                                                               config.l2_nof_slots_ahead,
                                                                               config.scs,
                                                                               *dependencies.bufferer_task_executor,
                                                                               dependencies.gateway,
                                                                               dependencies.last_msg_notifier});
  }
  auto decorators = fapi::create_decorators(decorator_cfg);

  // No decorators. Build a P7 with the given dependencies.
  if (!decorators) {
    return std::make_unique<mac_fapi_p7_sector_fastpath_adaptor_impl>(config, std::move(dependencies));
  }

  // Decorators present. Create new dependencies to point to the decorators.
  mac_fapi_p7_sector_fastpath_adaptor_dependencies adaptor_dependencies = {
      .gateway                = decorators->get_slot_message_gateway(),
      .last_msg_notifier      = decorators->get_slot_last_message_notifier(),
      .pm_mapper              = std::move(dependencies.pm_mapper),
      .part2_mapper           = std::move(dependencies.part2_mapper),
      .bufferer_task_executor = dependencies.bufferer_task_executor,
  };

  auto sector_adaptor =
      std::make_unique<mac_fapi_p7_sector_fastpath_adaptor_impl>(config, std::move(adaptor_dependencies));

  // Return the wrapper that keeps the life of the decorators.
  return std::make_unique<mac_fapi_adaptor_wrapper>(std::move(sector_adaptor), std::move(decorators));
}
