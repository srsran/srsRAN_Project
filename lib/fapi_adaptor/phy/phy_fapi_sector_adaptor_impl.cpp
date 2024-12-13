/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_sector_adaptor_impl.h"
#include "srsran/fapi/slot_last_message_notifier.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

/// Slot last message notifier dummy implementation.
class slot_last_message_notifier_dummy : public fapi::slot_last_message_notifier
{
public:
  void on_last_message(slot_point slot) override {}
};

} // namespace

static slot_last_message_notifier_dummy dummy_notifier;

/// Generates and returns a FAPI-to-PHY translator configuration from the given PHY adaptor configuration.
static fapi_to_phy_translator_config
generate_fapi_to_phy_translator_config(const phy_fapi_sector_adaptor_impl_config& config)
{
  fapi_to_phy_translator_config fapi_config;

  fapi_config.sector_id                  = config.sector_id;
  fapi_config.nof_slots_request_headroom = config.nof_slots_request_headroom;
  fapi_config.scs                        = config.scs;
  fapi_config.scs_common                 = config.scs_common;
  fapi_config.prach_cfg                  = config.prach_cfg;
  fapi_config.carrier_cfg                = config.carrier_cfg;
  fapi_config.prach_ports                = config.prach_ports;

  return fapi_config;
}

/// Generates and returns a FAPI-to-PHY translator dependencies from the given PHY adaptor dependencies.
static fapi_to_phy_translator_dependencies
generate_fapi_to_phy_translator_dependencies(phy_fapi_sector_adaptor_impl_dependencies&& dependencies)
{
  fapi_to_phy_translator_dependencies fapi_dependencies;

  fapi_dependencies.logger               = dependencies.logger;
  fapi_dependencies.dl_processor_pool    = dependencies.dl_processor_pool;
  fapi_dependencies.dl_rg_pool           = dependencies.dl_rg_pool;
  fapi_dependencies.dl_pdu_validator     = dependencies.dl_pdu_validator;
  fapi_dependencies.ul_request_processor = dependencies.ul_request_processor;
  fapi_dependencies.ul_rg_pool           = dependencies.ul_rg_pool;
  fapi_dependencies.ul_pdu_repository    = dependencies.ul_pdu_repository;
  fapi_dependencies.ul_pdu_validator     = dependencies.ul_pdu_validator;
  fapi_dependencies.pm_repo              = std::move(dependencies.pm_repo);
  fapi_dependencies.part2_repo           = std::move(dependencies.part2_repo);

  return fapi_dependencies;
}

phy_fapi_sector_adaptor_impl::phy_fapi_sector_adaptor_impl(const phy_fapi_sector_adaptor_impl_config&  config,
                                                           phy_fapi_sector_adaptor_impl_dependencies&& dependencies) :
  results_translator(*dependencies.logger),
  fapi_translator(generate_fapi_to_phy_translator_config(config),
                  generate_fapi_to_phy_translator_dependencies(std::move(dependencies))),
  time_translator(fapi_translator)
{
}

upper_phy_timing_notifier& phy_fapi_sector_adaptor_impl::get_timing_notifier()
{
  return time_translator;
}

void phy_fapi_sector_adaptor_impl::set_slot_time_message_notifier(
    fapi::slot_time_message_notifier& fapi_time_slot_notifier)
{
  time_translator.set_slot_time_message_notifier(fapi_time_slot_notifier);
}

void phy_fapi_sector_adaptor_impl::set_slot_error_message_notifier(
    fapi::slot_error_message_notifier& fapi_error_notifier)
{
  fapi_translator.set_slot_error_message_notifier(fapi_error_notifier);
  error_translator.set_slot_error_message_notifier(fapi_error_notifier);
}

void phy_fapi_sector_adaptor_impl::set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier)
{
  results_translator.set_slot_data_message_notifier(fapi_data_notifier);
}

fapi::slot_last_message_notifier& phy_fapi_sector_adaptor_impl::get_slot_last_message_notifier()
{
  return dummy_notifier;
}

fapi::slot_message_gateway& phy_fapi_sector_adaptor_impl::get_slot_message_gateway()
{
  return fapi_translator;
}

upper_phy_rx_results_notifier& phy_fapi_sector_adaptor_impl::get_rx_results_notifier()
{
  return results_translator;
}

upper_phy_error_notifier& phy_fapi_sector_adaptor_impl::get_error_notifier()
{
  return error_translator;
}
